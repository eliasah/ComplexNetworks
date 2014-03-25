/*
 * In case you of Abort trap : 6 error,
 * please make sure to remove -O2 option 
 * from the compiler and compile again
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <libgen.h>
#include <unistd.h>
#define NUM_COMMANDS 4
#define BUFSIZE 100
#define DEFAULT 0

int verbose = DEFAULT;

#define max(a,b)				\
    ({ __typeof__ (a) _a = (a);			\
	__typeof__ (b) _b = (b);		\
	_a > _b ? _a : _b; })

int get_bound_sup(const char *filepath) {
    FILE* fd = fopen(filepath,"r");
    if (fd == NULL) {
	printf("get_bound_sup : fatal\n");
	exit(-1);
    }
    char *result, line[BUFSIZE];
    int i = 0, tmp;
    while ((result = fgets(line,BUFSIZE,fd)) != NULL) {
	int rc;
	rc = sscanf(result,"%d",&tmp);
	i = max(i,tmp);
    }
    return i;
}

char* prepare_output_file(char *filepath) {
    char *tmp = malloc(sizeof(filepath));
    strcpy(tmp, filepath);
    tmp = strtok(tmp,".");
    if (verbose == 1)
	printf("%s\n",tmp);
    char *s1 = "set output '";
    char *s2 = ".pdf'";
    char *result = malloc(strlen(s1)+strlen(tmp)+strlen(s2)+1);
    strcpy(result,s1);
    strcat(result,tmp);
    strcat(result,s2);
    if (verbose == 1)
	printf("%s\n",result);
    return result;
}

char* prepare_to_plot(char *filepath) {
    char *s1 = "plot[][] '";
    char *s2 = "'";
    char *result = malloc(strlen(s1)+strlen(filepath)+strlen(s2)+1);
    strcpy(result,s1);
    strcat(result,filepath);
    strcat(result,s2);
    return result;
}

/*
 * This function concatenate two string and return it
 */
char* concat(char *s1, char *s2)
{
    char *result = malloc(strlen(s1)+strlen(s2)+1);//+1 for the zero-terminator
    
    // TODO CHECK MALLOC RETURN VALUE

    strcpy(result, s1);
    strcat(result, s2);
    return result;
}

int main(int argc,char** argv) {
    if (argc < 2) {
	printf("USAGE %s file.distribution\n", argv[0]);
	exit(1);
    }

    if (verbose == 1) printf("plot - %s -\n",argv[1]);
    char* arg1 = prepare_output_file(argv[1]);
    char* arg2 = prepare_to_plot(argv[1]);
    char* commandsForGnuplot[] = {"set logscale xy",
				  "set terminal pdf",
				  arg1,
				  arg2 };
    FILE* gnuplotPipe = popen ("gnuplot -persistent", "w");
    int i;    
    for (i=0; i < NUM_COMMANDS; i++) {
	fprintf(gnuplotPipe, "%s \n", commandsForGnuplot[i]);
    }
    if (verbose == 1) printf("end plot\n");
    return 0;
}
