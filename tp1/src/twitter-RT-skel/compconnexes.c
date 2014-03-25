#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "graph.h"
#include "bfs.h"

#define DEFAULT 0

struct graph *g;

int main(int argc,char** argv) {
    int opt = DEFAULT;
    if (argc < 2) {
	printf("USAGE : \t connexity fic.data\n");
	exit(-1);
    }
    
    FILE *fd = fopen(argv[1],"r");
    if (fd == NULL) {
	printf("Inexistant file");
	exit(-1);
    }
    
    if (argv[2] != NULL) {
	if (strcmp(argv[2],"-v") == 0) {
	    printf("VERBOSE MODE SET\n");
	    opt = 1;
	}
    } 
    // Initialize graph with the given file in argument
    init_graph(fd,opt);
    // Computer links and nodes
    compute_links_and_nodes(g,argv[1]);
    // Compute degrees
    compute_degrees(argv[1]);
    // Compute the adjacent array of the graph
    compute_adj_array();
 
    // print_graph();
    // Test the connexity of the graph
  
    int *rc = test_connexity(g); 
    if (rc[0] == 1) {
	printf("Le graphe est connexe.\n");
    }
    else {
	printf("Le graphe n'est pas connexe. Il possede %d composante(s) connexe(s) dont la taille de 3 plus grandes est %d, %d et %d\n",rc[0],rc[1],rc[2],rc[3]);
    }
  
    if (fclose(fd))
	perror("fclose error");
  
    return 1;
}
