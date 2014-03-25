#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#define BUFSIZE 100
#define DEFAULT 0

int *set,*rank, *visited, *cc_size, *root, *set_size;
int nodes,cc_count, res, cc1,cc2;

void init(char* fn) {
    int rc,tmp;
    char *result;
    char line[BUFSIZE];

    // check if file exists
    FILE *fd = fopen(fn,"rb");
    if (fd == NULL) {
	printf("cc-uf : Inexistant file\n");
	exit(-1);
    }
    
    // open file.n
    result = fgets(line,BUFSIZE,fd);
    if (result == NULL) {
	printf("cc-uf : fgets error\n");
	exit(-1);
    }
    
    // read node count from file.n
    rc = sscanf(result,"%d",&tmp);
    if (rc == 1) {
	nodes = tmp;
    }

    fclose(fd);
 
    int i;
    set      = malloc(nodes * sizeof(int));
    rank     = calloc(nodes, sizeof(int));
    visited  = calloc(nodes, sizeof(int));
    root     = calloc(nodes,sizeof(int));
    cc_size  = calloc(3, sizeof(int));
    set_size = malloc(nodes * sizeof(int));
    cc1 = 0;
    cc2 = 0;
    cc_count = 0;
    
    // init set
    for (i = 0;i< nodes;i++) {
	set[i] = i;
    }
}

/**
 * Compute the number of connexed componants and 
 * the size of the three biggest connexed componant
 *
 **/
void compute_cc_count() {
    int i;
    for (i = 0; i < nodes; i++) {
	if ( visited[set[i]] == 0 ) {
	    visited[set[i]]++;
	    cc_count++;
	}
	if ( set_size[i] > cc_size[0] ) {
	    cc_size[2] = cc_size[1];
	    cc_size[1] = cc_size[0];
	    cc_size[0] = set_size[i];
	}
	else if ( set_size[i] > cc_size[1] ) {
	    cc_size[2] = cc_size[1];
	    cc_size[1] = set_size[i];
	}
	else if ( set_size[i] > cc_size[2] )
	    cc_size[2] = set_size[i];
    }
}

int find(int u) {
    int p,v,tmp;
    p = u;
    while (set[p] != p) {
	p = set[p];
    }
    v = u;
    while (set[v] != v) {
	tmp = set[v];
	set[v] = p;
	v = tmp;
    }
    return p;
}

void link(int u, int v){
    if(rank[u] > rank[v]){
	set[v]=u;
    }
    else {
	set[u]=v;
	if(rank[u] == rank[v] && u!= v){
	    rank[v]++;
	}
    }
}

void union_set(u,v){
    link(find(u),find(v));
}



int main(int argc, char** argv) {
    int verbose = DEFAULT;

    if (argc < 4) {
	printf("USAGE: \t %s file.data file.n node1 node2 [-v]\n",argv[0]);
	printf("\t file.data \t - is a file representing the graph in a specific format\n");
	printf("\t file.n \t - is the file that contains the number of nodes in file.data\n");
	printf("\t node1 \t\t - test node number 1\n");
	printf("\t node2 \t\t - test node number 2\n");
	printf("\t -v \t\t - set verbose mode\n");
	exit(0);
    }
    
    char* path_data = argv[1]; 
    char* path_n    = argv[2];
    int node1       = atoi(argv[3]);
    int node2       = atoi(argv[4]);
    if (argv[5] != NULL && strcmp(argv[5],"-v") == 0)
	verbose = 1;

    int i;

    // Init structure
    init(path_n);
    
    // compute set and rank
    int l1,l2;
    char *tmp = malloc(BUFSIZE *sizeof(char));
    FILE *fdata = fopen(path_data,"rb");
    while((fgets(tmp,BUFSIZE,fdata)) != NULL){
	assert( sscanf(tmp,"%d %d",&l1,&l2) == 2 );
	// for each edge in G, unify set
	union_set(l1,l2);
    }
    fclose(fdata);
    
    for(i = 0 ; i < nodes ; i++){
	res = find(i);
	set[i] = res;
        root[set[i]]++;
	set_size[res] +=1;
	if(i == node1) cc1 = res;
	if(i == node2) cc2 = res;
    }

    
    compute_cc_count();
    float fraction = (float) 100 * cc_size[0] / nodes;
    if (verbose == 1) {
	printf("le nombre de composante connexe : %d\n",cc_count);
	printf("la taille de la plus grande composante connexe : %d\n",cc_size[0]);
	printf("la taille de la deuxieme plus grande composante connexe : %d\n",cc_size[1]);
	printf("la taille de la troisieme  grande composante connexe : %d\n",cc_size[2]);
	printf("la taille de la composante connexe à laquelle appartient le noeud test %d : %d\n",node1,set_size[cc1]);
	printf("la taille de la composante connexe à laquelle appartient le noeud test %d : %d\n",node2,set_size[cc2]);
	printf("la fraction de noeuds dans la plus grande composantes connexes est égale à %.2f%%\n", fraction);
    }
    printf("%d %d %d %d %d\n",cc_size[0],cc_size[1],cc_size[2],set_size[cc1],set_size[cc2]);
    
    return EXIT_SUCCESS;
    
}
