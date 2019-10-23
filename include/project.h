/**
 * CITS3402 Assigmment 2: All Pairs Shortest Path 
 * Floyd-Warshall Distributed Implementation
 *
 * File: project.h
 *
 * By:
 *   Ben Longbottom  | 22234771
 *   David Adams     | 22497769
 **/


#include<stdio.h>
#include<stdlib.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>
#include <getopt.h>
#include <sys/types.h>

#include"mpi.h"


typedef struct matrix {
    int size;
    int **array;
} MATRIX;


typedef struct _sub_matrix {
	int fullSize;
	int localSize;
	int vertexOffset;
	int **array;
} SUB_MATRIX;

//file io
int read_file_distributed(SUB_MATRIX *m, char *filepath);
int write_matrix_to_file(SUB_MATRIX *sm, char *filepath);
int get_local_vertex_count(int p, int pc, int nc);
int get_vertex_offset(int p, int pc, int nc);

//sequential file io
int read_file(MATRIX *m, char *filepath);

//memory functions
void *handle_malloc(size_t size);
void *handle_realloc(void *ptr, size_t size);
void free_sub_matrix(SUB_MATRIX *sm);

//utility functions
void copy_array(int * to, int * from, int length);
int min(int a, int b);
int vertex_in_proc(int n, int nc, int pc);

//display
void print_matrix(MATRIX *m);
void print_matrix_distributed(SUB_MATRIX * sm);
//void print_sub_matrix(SUB_MATRIX *m);
void print_array(int *arr, int length);

//floyd-warshall
int floyd_warshall(MATRIX * m);
int floyd_warshall_distributed(SUB_MATRIX * sm);

