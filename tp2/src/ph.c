#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#define BUFSIZE 100
#define DEFAULT 0

int *set,*rank, *visited, *cc_size, *root, *set_size, *deg, *degtmp,*p2,*distribution;
int **neighbors;
int nodes,cc_count, res, root_biggest;

/**
 * Define set structure
 *
 **/
typedef struct ens {
    unsigned int size;
    int *element;
    int *index;
} ens;

/**
 * Init set structure
 *
 **/
struct ens *init_ens(unsigned int size) {
    ens *e = malloc(sizeof(struct ens));
    e->size = 0;
    e->element = calloc(size,sizeof(int));
    e->index   = calloc(size,sizeof(int));
    return e;
}

/**
 * Delete node from set
 *
 **/
void del(struct ens *e,int node) {
    int i = e->index[node];
    e->size--;
    e->element[i] = e->element[e->size];
    e->index[e->element[i]] = i;
    e->index[node] = -1;    
}

/**
 * Add node to set
 *
 **/
void add(struct ens *e,int node) {
    e->element[e->size] = node;
    e->index[node] = e->size;
    e->size++;
}

/**
 * Check if set is empty
 *
 **/
int is_empty(struct ens *e) {
    return (e->size == 0);
}

/**
 * Initialize environement
 *
 */
void init(char* fn) {
    int rc,tmp;
    char *result;
    char line[BUFSIZE];

    // check if file exists
    FILE *fd = fopen(fn,"rb");
    if (fd == NULL) {
	printf("ph : Inexistant file\n");
	exit(-1);
    }
    
    // open file.n
    result = fgets(line,BUFSIZE,fd);
    if (result == NULL) {
	printf("ph : fgets error\n");
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
    deg   = calloc(nodes, sizeof(int));
    cc_count = 0;
    
    neighbors = calloc(nodes,sizeof(int *));
    for (i = 0;i<nodes;i++) {
	neighbors[i] = calloc(deg[i], sizeof(int));
    }

    // init set
    for (i = 0;i< nodes;i++) {
	set[i] = i;
    }
}

/**
 * Read degrees from file
 *
 **/
void compute_degrees(char *fdata) {
    FILE *file = fopen(fdata,"rb");
    int cpt = 0; int arg;
    int rc;
    char tmp[BUFSIZE];
    while (fgets(tmp,BUFSIZE,file) != NULL) {
	rc = sscanf(tmp,"%d\n",&arg);
	deg[cpt] = arg;
	cpt++;
    }
    fclose(file);

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
	    cc_size[0] = set_size[i];
	    root_biggest = set[i];
	}
    }

}

/**
 * Compute neighborhood graph
 *
 **/
void compute_neighbors(char *file_data) {

    int i, arg1,arg2;
    char line[BUFSIZE];
    FILE *fd = fopen(file_data,"rb");
    neighbors = calloc(nodes,sizeof(int *));
    for (i = 0;i < nodes;i++) {
	neighbors[i] = calloc(deg[i], sizeof(int));
    }
  
    int *tmp = calloc(nodes,sizeof(int));  
    
    while(fgets(line,BUFSIZE,fd) != NULL){
	 int rc;
	 rc = sscanf(line,"%d %d",&arg1,&arg2); 
	 if (rc == 2) {
	     neighbors[arg1][tmp[arg1]] = arg2;
	     neighbors[arg2][tmp[arg2]] = arg1;
	     tmp[arg1]++;
	     tmp[arg2]++;
	 }
	 else printf("error : compute_adj_array\n");
    }
    free(tmp);
}

/**
 * Compute nodes degrees distribution for periphery graph
 *
 **/
void distribution_periph() {
    int i;
    int *dist_deg = calloc(nodes,sizeof(int));
    for(i=0;i<nodes;i++){
	if (p2[i]==1) {
	    dist_deg[deg[i]]++;
	}
    }

    FILE *fd = fopen("graph-ph.d","wb");
    for(i = 0; i < nodes; i++){
	if (dist_deg[i] > 0)
	    fprintf(fd,"%d %d\n",i,dist_deg[i]);
    }
    fclose(fd);
}

void compute_degrees_distribution(){
    int i;
    distribution = calloc(nodes,sizeof(int));
    for ( i = 0 ; i < nodes;i++) {
	distribution[deg[i]]++;
    }
    
    FILE *fd = fopen("graph.d","wb");
    for ( i = 0 ; i < nodes;i++) {
	if (distribution[i] > 0)
	    fprintf(fd,"%d %d\n",i,distribution[i]);
    }
    fclose(fd);
}


/**
 * Compute periphery graph
 *
 **/
int periph(int k) {
    struct ens *ens1, *ens2;
    int x,periph = 0,nx,i,j;
    p2 = calloc(nodes,sizeof(int));

    // copy deg array into degtmp
    degtmp = calloc(nodes, sizeof(int));
    for (i = 0;i<nodes;i++) {
	degtmp[i] = deg[i];
    }

    for (j = 1 ;j < k ;j++) {
	
	ens1 = (struct ens *) init_ens(nodes);
	ens2 = (struct ens *) init_ens(nodes);
	// find nodes with deg = 1 or 2 and put them into ens1 and ens2
	
	// for (j = 1 ;j <= k ;j++) {
	for (i = 0 ; i < nodes;i++) {
	    if (set[i] == root_biggest && degtmp[i] == 1) {
		add(ens1,i);
	    }
	    else if (set[i] == root_biggest && degtmp[i] == 2) {
		add(ens2,i);
	    }
	}
	
	
	// remove nodes with deg = 1 from cc
	while (!is_empty(ens1)) {
	    x = ens1->element[0];
	    del(ens1,x);
	    periph++;
	    p2[x] = 1;
	    for (i = 0; i < deg[x];i++) {
		nx = neighbors[x][i];
		degtmp[nx]--;
		if (degtmp[nx] == 1) {
		    del(ens2,nx);
		    add(ens1,nx);
		}
		else if (degtmp[nx] == 2) {
		    add(ens2,nx);
		}
	    }
	}
	
	// remove nodes with deg = 2 from cc
	while(!is_empty(ens2)){
	    x = ens2->element[0];
	    del(ens2,x);
	    periph++;
	    p2[x] = 1;
	    for (i = 0; i < deg[x];i++) {
		nx = neighbors[x][i];
		degtmp[nx]--;
		if (degtmp[nx] == 2) {
		    add(ens2,nx);
		}
	    }
	}
    }
    if (k > 1)
	return periph/(k-1);
    return periph;
}

/**
 * Find procedure
 *
 **/
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

/**
 * Link procedure
 *
 **/
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

/**
 * Union procedure
 *
 **/
void union_set(u,v){
    link(find(u),find(v));
}


int main(int argc, char** argv) {
    int i,k = 2;
    
    if (argc < 4) {
	printf("USAGE: \t %s file.data file.n k [-v] [-d]\n",argv[0]);
	printf("\t file.data \t - is a file representing the graph in a specific format\n");
	printf("\t file.n \t - is the file that contains the number of nodes in file.data\n");
	printf("\t file.deg \t - is the file that contains the degrees for each node\n");
	printf("\t k \t\t - k-periphy\n");
	printf("\t -d \t\t - compute degrees distribution in periphery\n");
	exit(0);
    }
    
    char *path_data = argv[1]; 
    char *path_n    = argv[2];
    char *path_deg  = argv[3];
    
    if (argv[4] != NULL) {
	k = atoi(argv[4]);
    }
    else k = 1;
    assert( k >= 1);

    // Init structure
    init(path_n);
    
    compute_degrees(path_deg);
    compute_neighbors(path_data);
   
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
    }

    compute_cc_count();
    
    int p = periph(k);
    
    distribution_periph();
    compute_degrees_distribution();

    float fraction = (float) 100 * (float) p / (float) cc_size[0];
    printf("%.2f \n",fraction);
    return EXIT_SUCCESS;
    
}
