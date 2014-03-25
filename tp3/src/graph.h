#ifndef GRAPH_H
#define GRAPH_H
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "bool.h"

typedef struct graph{
    int **neighbors;
    int *degres;
    int *distribution;
    int lines;
    int nodes;
} graph;

char *concat(char *s1,char *s2);
void process_vertex(int v);
void process_edge(int x,int y);
bool valid_edge(int e);
void compute_links_and_nodes(struct graph *g,char *output);
void compute_degrees(char* output);
void compute_adj_array();
void compute_degrees_distribution();
void init_graph(FILE* fd,int opt);
void destroy_graph();
void print_graph();
#endif
