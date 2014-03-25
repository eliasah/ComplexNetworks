#ifndef QUEUE_H
#define QUEUE_H

#include <stdio.h>


/* Ce fichier définit les interfaces que vous devez implémenter. */

/* Le C est bizarre comme-ça, il aime bien qu'on lui mentionne les
   structures dont on se sert même si on ne les définit pas. */
struct queue;
struct node;
struct iovec;

int is_empty(const struct queue* q);
void queue_print_node(const struct node* n);
void queue_print(const struct queue* q);

/* Création et destruction. */

struct queue *queue_new(size_t size);
void queue_destroy(struct queue *queue);

/* Enfilage et défilage. */

int enqueue(struct queue *queue, const void *buf, size_t size, int blocking);
ssize_t dequeue(struct queue *queue, void *buf, size_t bufsize, int blocking);

/* Enfilage et défilage multiples. */

int enqueue_multiple(struct queue *queue, const struct iovec *iov, int iovcnt,
                     int blocking);
int dequeue_multiple(struct queue *queue, struct iovec *iov, int iovcnt,
                     int blocking);

#endif
