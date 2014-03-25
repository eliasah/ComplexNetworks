#include <stdlib.h>
#include <stdio.h>
#include <time.h>

void usage(char *s) {
    printf("%s : not enough arguments\n",s);
    exit(0);
}

void generate(int nodes, int links) {
    int i;
    srandom(time(NULL));
    for (i = 0; i < links; i++) {
	printf("%d %d\n",(int) random()%nodes,(int) random()%nodes);
    }
}


int main(int argc, char **argv) {
    if (argc < 3)
	usage(argv[0]);

    int nodes = atoi(argv[1]);
    int links = atoi(argv[2]);

    generate(nodes,links);
    
    return 1;

}
