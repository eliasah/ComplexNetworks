#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "graph.h"
#include "bfs.h"

#define DEFAULT 0
#define VERBOSE 1

struct graph *g;

int main(int argc,char** argv) {
    int opt = DEFAULT;
  if (argc < 3) {
    printf("USAGE : \t commande fic.data node_number\n");
    exit(-1);
  }

  FILE *fd = fopen(argv[1],"r");
  if (fd == NULL) {
    printf("Inexistant file");
    exit(-1);
  }

  if (argv[3] != NULL && strcmp(argv[3],"-v")) {
      opt = VERBOSE;
  }
  init_graph(fd,opt);
  

  compute_degrees(argv[1]);
  compute_adj_array();
  int i = atoi(argv[2]);
  compute_avg_distance(g,i,argv[1]);

  if (fclose(fd))
    perror("fclose error");
  
  return 1;
}
