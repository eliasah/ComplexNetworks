#include "queue.h"
#include "bool.h"
#include "bfs.h"
#include <stdlib.h>
#include <libgen.h>

#define max(a,b)				\
    ({ __typeof__ (a) _a = (a);			\
	__typeof__ (b) _b = (b);		\
	_a > _b ? _a : _b; })

int *parent;           // array of parents considering a certain vertex after bfs
int *distance;         // array of distance considering a certain vertex
int *distribution;
bool processed = FALSE;
int cc_count = 1;
int cc_max_size1 = 0;  // biggest CC's numbers of nodes
int cc_max_size2 = 0;  // second biggest CC's numbers of nodes
int cc_max_size3 = 0;  // third biggest CC's numbers of nodes
int cpt;               // number of nodes in current CC
int *result;
char* outputname;

/*
 * This function considers a graph and a vertex and computes with a bfs
 * the distance between the considered vertex and the rest of the vertices
 * in the same CC returns the average distance
 */
int compute_avg_distance(graph *g, int vertex,char* output) {
    if (output == NULL)
	outputname = "graph.distribution";
    else {
	char *tmp = basename (output);
	tmp = strtok(tmp,".");
	outputname = tmp;
    }
    bfs(g,vertex);
    float sum = 0;
    float avg = 0;
    int i;
    
    for ( i = 0;i < g->nodes;i++) {
	if (distance[i] != -1) {
	    // printf("dist(%d) = %d\n",i,distance[i]);
	    sum = sum + distance[i];
	    distribution[distance[i]]++;
	    // fprintf(fd, "%d\n", distance[i]);
	}
    }
    avg = sum / cpt;
    printf("La distance moyenne  : %f \n",avg);
    compute_distance_distribution(g);
    return avg;
}

/* 
 * This function test the connexity of a given graph g along with size of the biggest 3 CC.
 * returns the count of CC in the given graph
 */
int* test_connexity(graph *g) {
    int i;
    result = calloc(4,sizeof(int));
    
    if (processed == FALSE) {
	bfs(g,0);
	processed = TRUE;
    }
    
    for (i = 0; i < g->nodes; i++) {
	if (parent[i] == -1) {
	    cc_count++;
	    bfs(g,i);
	}
    }
    result[0] = cc_count;
    result[1] = cc_max_size1;
    result[2] = cc_max_size2;
    result[3] = cc_max_size3;
    
    return result;
}

/*
 * This function compute a BFS for a given graph considering a starting vertex
 */
void bfs(graph *g, int start) {
    cpt = 0;
    struct queue *q = queue_new(g->nodes);
    int v = start;
 
    int i,z,u;
    if (processed == FALSE) {
	parent = malloc(sizeof(int) * g->nodes);
	distance = malloc(sizeof(int) * g->nodes);
	distribution = calloc(g->nodes,sizeof(int));
	for (i = 0;i<g->nodes;i++) {
	    parent[i] = -1;
	    distance[i] = -1;
	}
    }

    enqueue(q,&v,sizeof(int),0);
    parent[v] = v;
    distance[v] = 0;
    while (!is_empty(q) > 0) {
	dequeue(q,&u,sizeof(int),0);
	cpt++;
	for (i = 0;i < g->degres[u];i++) {
	    z = g->neighbors[u][i];
	    if (parent[z]== -1) {
		enqueue(q,&z,sizeof(z),0);
		parent[z] = u;
		distance[z] = distance [u] + 1;
		// printf("dist(%d) = %d\n",z,distance[z]);
	    }
	}
    }
 
    if (cpt > cc_max_size1) // printf("|CC| = %d\n",cpt);
	cc_max_size1 = cpt;
    else if (cpt > cc_max_size2)
	cc_max_size2 = cpt;
    else if (cpt > cc_max_size3)
	cc_max_size3 = cpt;
 
    processed = TRUE;
    queue_destroy(q);
 
}


void compute_distance_distribution(graph *g){
    int i;
    char *output = concat(outputname,".distribution"); 
    FILE* fd = fopen(output,"wb");
    for ( i = 0 ; i < g->nodes;i++) {
	if (distribution[i] > 0)
	    fprintf(fd,"%d\n",distribution[i]);
    }
    free(output);
}


void bfs_clean() {
    free(parent);
    free(distance);
    free(result);
}
