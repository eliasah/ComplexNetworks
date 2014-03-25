#ifndef BFS_H
#define BFS_H

#include <stdio.h>
#include <stdlib.h>
#include "graph.h"

int compute_avg_distance(graph *g, int vertex,char *output);
int* test_connexity(graph *g);
void compute_distance_distribution(graph *g);
void bfs(graph *g, int start);
void bfs_clean();

#endif
