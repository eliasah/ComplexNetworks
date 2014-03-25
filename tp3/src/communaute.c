#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define DEFAULT 1
#define BUFSIZE 1024

int links,nodes,verbose = DEFAULT;
int *comm, *deg, *aij;
int **matrix;

int compare (const void * a, const void * b) {
   return ( *(int*)b - *(int*)a );
}

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

void compute_adj_matrix(char *filepath) {
    FILE *file = fopen(filepath,"rb");
    char tmp[BUFSIZE];
    int u,v;

    while (fgets(tmp,BUFSIZE,file) != NULL) {
	sscanf(tmp,"%d %d\n",&u,&v);
	matrix[u][v] = 1;
	matrix[v][u] = 1;
	links++;
    }

    fclose(file);
}

void init(char *fn,char *path_data,char *path_deg) {
    int i,j;
    links = 0;
    int rc,tmp;
    char *result;
    char line[BUFSIZE];

    // check if file exists
    FILE *fd = fopen(fn,"rb");
    if (fd == NULL) {
	printf("Inexistant file\n");
	exit(-1);
    }
    
    // open file.n
    result = fgets(line,BUFSIZE,fd);
    if (result == NULL) {
	printf("fgets error\n");
	exit(-1);
    }
    
    // read node count from file.n
    rc = sscanf(result,"%d",&tmp);
    if (rc == 1) {
	nodes = tmp;
    }

    fclose(fd);

    comm = malloc(nodes * sizeof(int));
    matrix = calloc(nodes,sizeof(int *));
    deg = calloc(nodes,sizeof(int));
    aij = calloc(nodes,sizeof(int));

    // init array comm
    for (i = 0; i < nodes;i++) {
	matrix[i] = calloc(nodes, sizeof(int));
	comm[i] = i;
    }

    compute_adj_matrix(path_data);
    compute_degrees(path_deg);

    for (i = 0; i < nodes;i++)
	for (j = 0; j < nodes;j++)
	    aij[i] += matrix[i][j];
	    
}

int delta(int i,int j) {
    return (comm[i] == comm[j]);
}

float deltaQ(int i,int j) {
    float eij = (float) matrix[i][j] / (float) (2 * links);
    float ai = (float) aij[i] / (float) (2 * links);
    float aj = (float) aij[j] / (float) (2 * links);
    return (float) (2 * (eij - (float) (ai * aj)));
}

void update_matrix(int i,int j) {
    int k,tmp = 0;
    aij[i] = 0;
    aij[j] = 0;

    tmp = matrix[i][i] + matrix[j][j] + matrix[i][j];
    matrix[i][i] = tmp;
    matrix[j][j] = tmp;
    matrix[i][j] = tmp; //0 ou tmp;
    matrix[j][i] = tmp; //0 ou tmp;

    for (k = 0;k < nodes; k++) {
	if (k != i && k != j) {
	    tmp = matrix[j][k] + matrix[i][k];
	    matrix[i][k] = tmp;
	    matrix[k][i] = tmp;
	    matrix[j][k] = tmp;
	    matrix[k][j] = tmp;
	}
	aij[i] += matrix[i][k];
	aij[j] += matrix[j][k];
    }
}

void fusion(int i,int j) {
    int k;
    for (k = 0;k<nodes;k++) {
	if (comm[k] == j) comm[k] = i;
    }
}

void algorithm(char *fdata) {
    FILE *file = fopen(fdata,"rb");
    float delta_max = 1;
    int u,v;
    float dq = 0;
    int i = 0,j = 0;
    char tmp[BUFSIZE];
    
    while (delta_max > 0) {
	delta_max = 0.0;
	fseek(file, 0L, SEEK_SET);
	while (fgets(tmp,BUFSIZE,file) != NULL) {
	    sscanf(tmp,"%d %d\n",&u,&v);
	    if (comm[u] != comm[v]) {
		dq = deltaQ(u,v);
		if (dq > delta_max) {
		    // printf("DeltaQ =  %.10f \33[2K\r",dq);
		    delta_max = dq;
		    i = comm[u];
		    j = comm[v];
		}
	    }
	}
	update_matrix(i,j);
	fusion(i,j);
    }
    fclose(file);
}

float modularity() {
    float q = 0;
    int i,j;

    for (i = 0; i<nodes; i++) {
	for (j = 0; j<nodes; j++) {
	    if (i == j)
		continue;
	    q += ((float) matrix[i][j] - ((float) (deg[i] * deg[j]) / (float) (2 * links))) * (float) delta(i,j);
	}
    }
    return ((float) q / ((float) 2 * (float) links));
}



int main(int argc,char **argv) {
    int i,cpt = 0, node1, node2;
    
    if (argc < 5) {
	printf("USAGE: \t %s file.data file.n file.deg node1 node2\n",argv[0]);
	printf("\t file.data \t - is a file representing the graph in a specific format\n");
	printf("\t file.n \t - is the file that contains the number of nodes in file.data\n");
	printf("\t node1, node2 \t - integers\n");
	exit(0);
    }
    
    char *path_data = argv[1]; 
    char *path_n    = argv[2];
    char *path_deg  = argv[3];
    if (argv[4] != NULL)
	node1 = atoi(argv[4]);
    else node1 = -1;
    if (argv[5] != NULL)
	node2 = atoi(argv[5]);
    else node2 = -1;

    // Init structure
    init(path_n,path_data,path_deg); 

    // Compute Communities
    algorithm(path_data);

    // Count Communities
    int *tmp = calloc(nodes,sizeof(int));
    for (i = 0 ;i <nodes;i++) {
	tmp[comm[i]]++;
    }
    for (i = 0 ;i <nodes;i++) {
	if (tmp[i] != 0)
	    cpt++;
    }

    // Re-init adj matrix
    free(matrix);
    matrix = calloc(nodes,sizeof(int *));
    for (i = 0; i < nodes;i++) {
	matrix[i] = calloc(nodes, sizeof(int));
    }
    compute_adj_matrix(path_data);
    
    // Compute modularity
    float q = modularity();

    if (node1 != -1 && node2 != -1) {
	if (verbose ==1) {
	    if (comm[node1] == comm[node2]) printf("%d and %d are in the same community (%d)\n",node1,node2,comm[node1]);
	    else printf("%d and %d are not in the same community. comm[%d] = %d, comm[%d] = %d\n",node1,node2,node1,comm[node1],node2,comm[node2]);
	}
	else printf("%d ",comm[node1] == comm[node2]);
    }

    if (verbose == 1) printf("Size of comm[%d] : %d\n",comm[node1],tmp[comm[node1]]);
    else printf("%d ",tmp[comm[node1]]);
    // Print the size of the 10 biggest communities
    if (argv[6] != NULL && strcmp(argv[6],"-d") == 0) {
	qsort(tmp, nodes, sizeof(int), compare);
	if (verbose == 1) printf("Size of the 10 biggest communities : ");
	for (i = 0 ;i< 10;i++) {
	    printf("%d ",tmp[i]);
	}
	if (verbose == 1) printf("\n");
    }
   
    if (verbose == 1 ) printf("q = %.10f\n",q);
    if (verbose == 1 ) printf("c = %d\n",cpt);

    printf("\n");

}
