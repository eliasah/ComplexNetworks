#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "graph.h"

#define DEFAULT 0

struct graph *g;

int main(int argc,char** argv) {
    int opt = DEFAULT;
    if (argc < 2) {
	printf("USAGE : \t %s fic.data\n",argv[0]);
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
   
    if (fclose(fd))
	perror("fclose error");
  
    return 1;
}
