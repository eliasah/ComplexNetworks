#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "graph.h"
#include "bool.h"
#include "libgen.h"

#define BUFSIZE 100
#define DEFAULT 0

#define max(a,b)				\
    ({ __typeof__ (a) _a = (a);			\
	__typeof__ (b) _b = (b);		\
	_a > _b ? _a : _b; })

#define is_comment(a)				\
    startsWith("#",a)

struct graph *g;
FILE *fd;
char line[BUFSIZE], *result;
int arg1,arg2;
int verbose = DEFAULT;
char *outputname;

/*
 * This function concatenate two string and return it
 */
char* concat(char *s1, char *s2) {
    char *result = malloc(strlen(s1)+strlen(s2)+1);//+1 for the zero-terminator
    
    // TODO CHECK MALLOC RETURN VALUE

    strcpy(result, s1);
    strcat(result, s2);
    return result;
}

void process_vertex(int v) {
    printf("processed vertex %d\n",v);
}

void process_edge(int x, int y){
    printf("processed edge (%d,%d)\n",x,y);
}

bool valid_edge(int e) {
    return (TRUE);
}

bool startsWith(const char *pre, const char *str) {
    size_t lenpre = strlen(pre),
	lenstr = strlen(str);
    return lenstr < lenpre ? 0 : strncmp(pre, str, lenpre) == 0;
}


void print_graph() {
    int i,j;
    
    for (i=0; i<g->nodes; i++) {
	printf("%d: ",i);
	for (j=0; j<g->degres[i]; j++)
	    printf(" %d",g->neighbors[i][j]);
	printf("\n");
    }
}

void init_graph(FILE* file,int opt) {
    verbose = opt;
    g = malloc(sizeof(struct graph));
    g->lines = 0;
    g->nodes = 0;
    fd = file;
}

void compute_links_and_nodes(struct graph *g,char *output) {
    if (verbose == 1) printf("Calcul du nombre de liens et de noeuds\n");
    if (output == NULL)
	outputname = "graph.n";
    else {
	char *tmp = basename (output);
	tmp = strtok(tmp,".");
	outputname = tmp;
    }    
    
    while ((result = fgets(line,BUFSIZE,fd)) != NULL) {
	int rc;
	if (is_comment(result)) {
	    continue;
	}
	else {
	    rc = sscanf(result,"%d %d",&arg1,&arg2);
	    if (rc == 2) {
		g->nodes = max(g->nodes,max(arg1,arg2));
		g->lines++;
	    }
	    // else printf("error\n");
	}
    }
    g->nodes+=1;
  
    if (verbose == 1) printf("Nombre de liens : \t %d\n",g->lines);
    if (verbose == 1) printf("Nombre de noeuds : \t %d\n",g->nodes);

    char *tmp1 = concat("./",outputname);
    mkdir(tmp1,0700);
    chdir(tmp1);
    output = concat(outputname,".n");
    FILE* fd = fopen(output,"wb");
    fprintf(fd,"%d\n",g->nodes);
    free(output);
    free(tmp1);
}

void compute_degrees(char* output) {
    if (output == NULL)
	outputname = "graph.distribution";
    else {
	char *tmp = basename (output);
	tmp = strtok(tmp,".");
	outputname = tmp;
    }
    if (g->nodes == 0)
	compute_links_and_nodes(g,output);
 
    if (verbose == 1) printf("Calcul des degrés pour chaque noeuds\n");
    g->degres = calloc(g->nodes,sizeof(int));
    g->distribution = calloc(g->nodes,sizeof(int));
    
    fseek(fd, 0L, SEEK_SET);
    while ((result = fgets(line,BUFSIZE,fd)) != NULL) {
	int rc;
	if (is_comment(result)) {
	    continue;
	}
	else {
	    rc = sscanf(result,"%d %d",&arg1,&arg2);
	    if (rc == 2) {
		g->degres[arg1]++;
		g->degres[arg2]++;
	    }
	    // else printf("error\n");
	}
    }
    
    int i;
    output = concat(outputname,".deg");
    FILE* fd = fopen(output,"wb");
    for ( i = 0 ; i < g->nodes ;i++) {
	// if (g->degres[i] > 0)
	fprintf(fd,"%d\n",g->degres[i]);
    }
    free(output);
    // free(tmp1);

}

bool contains(int *array,int val) {
    int i;
    for (i = 0;i< sizeof(array)/sizeof(int);i++) {
	if (array[i] == val)
	    return TRUE;
    }
    return FALSE;
}

void compute_adj_array() {
    int i;

    if (verbose == 1)
	printf("Calcul du tableau d'adjacence du graphe\n");
    
    g->neighbors = calloc(g->nodes,sizeof(int *));
    for (i = 0;i<g->nodes;i++) {
	g->neighbors[i] = calloc(g->degres[i], sizeof(int));
    }
  
    int *tmp = calloc(g->nodes+1,sizeof(int));
    fseek(fd, 0L,SEEK_SET);
 
    int *tab = calloc(g->nodes,sizeof(int));
    
    while(fgets(line,BUFSIZE,fd) != NULL){
	 int rc;
	 if (is_comment(line)) {
	     continue;
	 }
	 else {
	     rc = sscanf(line,"%d %d",&arg1,&arg2); 
	     if (rc == 2) {
		 if(arg1!=arg2){
		     g->neighbors[arg1][tab[arg1] - tmp[arg1]] = arg2;
		     g->neighbors[arg2][tab[arg2] - tmp[arg2]] = arg1;
		     tmp[arg1]--;
		     tmp[arg2]--;
		 }
	     }
	     else printf("error : compute_adj_array\n");
	 }
     }

    free(tmp);
    if (verbose == 1) print_graph();
}

void compute_degrees_distribution(){
    int i;
    if (verbose == 1) printf("Calcul de la distribution des degrés\n");
    for ( i = 0 ; i < g->nodes;i++) {
	g->distribution[g->degres[i]]++;
    }
}

void destroy_graph() {
    free(result);
}
