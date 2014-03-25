#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#define DEFAULT 0
#define BUFSIZE 1024

int *array, *distribution;

void usage(char *s) {
    printf("%s : not enough arguments\n",s);
    exit(0);
}

void exchange(int i1,int i2) {
    int tmp = array[i1];
    array[i1] = array[i2];
    array[i2] = tmp;
}

void generate(int nodes, int links) {
    int i = 2 * links;
    srandom(time(NULL));
    while (i > 0) {
	int u = random() % i;
	int v  = random() % (i - 1);
	exchange(v,i-1);
	exchange(u,i);
	// write lins between T[i] and T[i-1]
	printf("%d %d\n", distribution[u], distribution[v]);
	i = i - 2;
    }
}

void read_distribution(char *path) {
    FILE *file = fopen(path,"rb");
    int cpt = 0;
    int arg1, arg2;
    int rc;
    char tmp[BUFSIZE];
    int i, j, k = 0;
    while (fgets(tmp,BUFSIZE,file) != NULL) {
	rc = sscanf(tmp,"%d %d\n",&arg1,&arg2);
	for (i = 0; i < arg2; i++) {
	    for (j = 0 ; j < arg1; j++) {
		distribution[cpt] = k;
		cpt++;
	    }
	    k++;
	}
    }
    fclose(file);
}

int main(int argc, char **argv) {
    if (argc < 4)
	usage(argv[0]);

    int nodes = atoi(argv[1]);
    int links = atoi(argv[2]);
    char *file = argv[3];
    
    // init data structures
    array = malloc(2 * links * sizeof(int));
    distribution = malloc(2 * links * sizeof(int));

    read_distribution(file);
  
    generate(nodes,links);
    
    return 1;

}
