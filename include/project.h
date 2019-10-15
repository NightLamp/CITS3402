#include<stdio.h>
#include<stdlib.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>
#include <getopt.h>
#include <sys/types.h>


typedef struct matrix {
    int size;
    int **array;
} MATRIX;

//reading the file
int read_file(MATRIX* m, char *filepath);

//memory functions
void *handle_malloc(size_t size);
void *handle_realloc(void *ptr, size_t size);

//debug
void debug();
void print_matrix(MATRIX *m);

//floyd-warshall
int min(int a, int b);
int floyd_warshall(MATRIX * m);
