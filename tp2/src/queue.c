#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>
#include <unistd.h>
#include <sys/uio.h>
#include "queue.h"

struct queue {
    pthread_mutex_t mutex;         // mutex
    pthread_cond_t cond_not_empty; // condition variable 1 : cond_not_empty
    pthread_cond_t cond_not_full;  // condition variable 2 : cond_not_full
    size_t length;                 // occupied space in queue
    size_t capacity;               // maximum capacity of the queue
    struct node *head,*tail;       // pointers to head element and tail element
    void *data;                    // buffer
};

struct node {
    size_t size;              // size of data
    struct node* next;        // next node
};

int is_empty(const struct queue* q) {
    return q->length == 0;
}

void queue_print_node(const struct node* n) {
    if(n) printf("size = %zu\n", n->size);
    else printf("Can not print NULL struct \n");
}

void queue_print(const struct queue* q) {
    struct node* n = NULL;
    printf("queue->data = %d\n",(int)(*(int*)q->data));
    if(q) {
	for(n = q->head;n; n = n->next ) {
	    queue_print_node(n);
	}
    }
    printf("------------------\n");
}

struct queue* queue_new(size_t size) {
    int m,rc;
    struct queue* q = malloc(sizeof(struct queue));
  
    if(q == NULL) {
	printf("in %s LINE: %d, malloc() failed\n",__FILE__,__LINE__);
    }

    m = pthread_mutex_init(&q->mutex,NULL);
    if (m != 0) {
	perror("failed to init mutex in queue_new()\n");
	return NULL;
    }    

    rc = pthread_cond_init(&q->cond_not_empty,NULL);
    if (rc != 0) {
	perror("failed to init cond_not_empty in queue_new()\n");
	return NULL;
    }
    rc = pthread_cond_init(&q->cond_not_full,NULL);
    if (rc != 0) {
	perror("failed to init cond_not_full in queue_new()\n");
	return NULL;
    }

    q->head = q->tail = NULL;
    q->length = 0;
    q->capacity = size;
    q->data = malloc(size);
    
    return q;
}

int enqueue(struct queue *queue,const void *buf,size_t size, int blocking) {
    int rc, avail;
    struct node* n;
    
    // MUTEX LOCKING :
    rc = pthread_mutex_lock(&queue->mutex);
    if (rc != 0) {
	perror("in dequeue mutex lock failed\n");
	return -1;
    }

    n = malloc(sizeof(struct node));
    if(n == NULL) {
	printf("in %s, %s: malloc() failed\n", __FILE__, "enqueue");
	pthread_mutex_unlock(&queue->mutex);
	return -1; 
    }
    n->size = size;
    n->next = NULL;
    
    avail = queue->capacity - queue->length; // available space left on queue

    if (queue == NULL) {
	printf("queue not initialized\n");
	free(n);
	pthread_mutex_unlock(&queue->mutex);
	return -1;
    }


    // UNBLOCKING QUEUE :
    if (blocking == 0) {
	if (avail < size) {
	    pthread_mutex_unlock(&queue->mutex);
	    free(n);
	    errno = EAGAIN;
	    return -1;
	}

	if (is_empty(queue)) {
	    queue->head = n;
	    queue->tail = n;
	    memmove(queue->data,buf,size);
	    queue->length = size;
	    pthread_mutex_unlock(&queue->mutex);
	    return size;
	}
	else {
	    queue->tail->next = n;
	    memmove(queue->data+queue->length,buf,size);
	    queue->tail = n;
	    queue->length = queue->length + size;
	    pthread_mutex_unlock(&queue->mutex);
	    return size;
	}

    }

    // BLOCKING QUEUE :
    else if (blocking == 1) {
	// BLOCKING CONDITION :
	if (avail < size) {
	    rc = pthread_cond_wait(&queue->cond_not_full, &queue->mutex);
	    if (rc != 0) {
		perror("in enqueue : pthread_cond_wait");
		exit(-1);
	    }
	    //printf("in enqueue : waiting...\n");
	}    
      
	if(is_empty(queue)) {
	    //printf("in enqueue: queue is empty, enqueue first element\n");
	    queue->head = n;
	    queue->tail = n;
	    memmove(queue->data,buf,size);
	    queue->length = size;

	    rc = pthread_cond_broadcast(&queue->cond_not_empty);
	    if (rc != 0) {
		perror("in enqueue : pthread cond signal failed!\n");
		exit(-1);
	    }

	    //printf("in enqueue: unlock mutex(1)\n");
	    pthread_mutex_unlock(&queue->mutex); 
	    return size;
	}
	else {
	    //printf("in enqueue: queue not empty, enqueue element in tail\n");
	    queue->tail->next = n;
	    memcpy(queue->data+queue->length,buf,size);
	    queue->tail = n;
	    queue->length = queue->length + size;

	    //printf("in enqueue : broadcasting queue not empty...\n");
	    rc = pthread_cond_broadcast(&queue->cond_not_empty);
	    if (rc != 0) {
		perror("in enqueue : pthread cond signal failed!\n");
		exit(-1);
	    }
	    //printf("in enqueue: unlock mutex(2)\n");
	    pthread_mutex_unlock(&queue->mutex);
	    return size;
	}
    }
    
    // MUTEX UNLOCK
    rc = pthread_mutex_unlock(&queue->mutex);
    if (rc != 0) {
	perror("in enqueue : pthread mutex unlock failed!\n");
	exit(-1);
    }
    return 0;
}

ssize_t dequeue(struct queue *queue, void *buf, size_t bufsize, int blocking) {
    int rc;
    struct node* n;
    
    // MUTEX LOCK
    rc = pthread_mutex_lock(&queue->mutex);
    if (rc != 0) {
	perror("in dequeue mutex lock failed\n");
	return -1;
    }
    
    if(queue == NULL) {
	perror("queue isn't initialized\n");
	pthread_mutex_unlock(&queue->mutex);
	return 0;
    }
    
    // UNBLOCKING QUEUE : 
    if (blocking == 0){
	if(queue->head == NULL) {
	    pthread_mutex_unlock(&queue->mutex);
	    errno = EAGAIN;
	    return -1;
	}
    
	if (bufsize < queue->head->size) {
	    pthread_mutex_unlock(&queue->mutex);
	    errno = EMSGSIZE;
	    return -1;
	}

	// memcpy risks overlapping, while memmove avoids this problem
	memmove(buf,queue->data,bufsize);
	memmove(queue->data, queue->data + queue->head->size,queue->length - queue->head->size);
	queue->length = queue->length - queue->head->size;    

	n = queue->head;
	queue->head = queue->head->next;
	free(n);

	// MUTEX UNLOCK
	rc = pthread_mutex_unlock(&queue->mutex);
	if (rc != 0) {
	    perror("in dequeue mutex unlock failed");
	    return -1;
	}
	return bufsize;
    }

    // BLOCKING QUEUE :
    else if (blocking == 1) {
	if (is_empty(queue)) {
	    //printf("in dequeue : waiting...\n");	   
	    // sleep(10);
	    rc = pthread_cond_wait(&queue->cond_not_empty, &queue->mutex);
	    if (rc != 0) {
		perror("in dequeue : pthread_cond_wait");
		exit(-1);
	    }
	}
	
	if (bufsize < queue->head->size) {
	    pthread_mutex_unlock(&queue->mutex);
	    errno = EMSGSIZE;
	    return -1;
	}

	// memcpy risks overlapping, while memmove avoids this problem
	memmove(buf,queue->data,bufsize);
	// printf("in dequeue : queue->data = %d, buf = %d\n",(int)(*(int*)queue->data),(int)(*(int*)buf));
	memmove(queue->data, queue->data + queue->head->size,queue->length - queue->head->size);
	queue->length = queue->length - queue->head->size;    

	n = queue->head;
	queue->head = queue->head->next;
	free(n);

	//printf("in dequeue : signalize condition space available\n");
	rc = pthread_cond_broadcast(&queue->cond_not_full);
	if (rc != 0) {
	    perror("in dequeue cond signal failed");
	    return -1;
	}

       	// MUTEX UNLOCK :
	rc = pthread_mutex_unlock(&queue->mutex);
	if (rc != 0) {
	    perror("in dequeue mutex unlock failed");
	    return -1;
	}
	return bufsize;
    }
    
    rc = pthread_mutex_unlock(&queue->mutex);
    if (rc != 0) {
	perror("in dequeue mutex unlock failed\n");
	return -1;
    }
    return -1;
}

void queue_destroy(struct queue* queue) {
    //printf("destroying queue");
    while(queue->head) {
	dequeue(queue,queue->head,queue->head->size,0);
    }
  

    pthread_mutex_destroy(&queue->mutex);
    pthread_cond_destroy(&queue->cond_not_empty);
    pthread_cond_destroy(&queue->cond_not_full);
    free(queue->data);
}

/* Enfilage et défilage multiples. */
int enqueue_multiple(struct queue *queue, const struct iovec *iov, int iovcnt, int blocking) {
    int i,rc ;
    for (i=0;i<iovcnt;i++) {
	rc = enqueue(queue,iov[i].iov_base,iov[i].iov_len,blocking);
	if (rc != iov[i].iov_len) return -1;
    }
    return i;
}
int dequeue_multiple(struct queue *queue, struct iovec *iov, int iovcnt, int blocking) {
    int i,rc ;
    for (i=0;i<iovcnt;i++) {
	rc = dequeue(queue,iov[i].iov_base,iov[i].iov_len,blocking);
	if (rc != iov[i].iov_len) return -1;
    }
    return i;
}


