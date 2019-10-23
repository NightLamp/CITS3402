/**
 * CITS3402 Assigmment 2: All Pairs Shortest Path 
 * Floyd-Warshall Distributed Implementation
 *
 * File: utill.c
 *
 * By:
 *   Ben Longbottom  | 22234771
 *   David Adams     | 22497769
 **/


#include"project.h"



//----------------------------------------------------------------------------//
// memory funcs

void *handle_malloc(size_t size) {
    void *res = malloc(size);
    if (res == NULL) {
        fprintf(stderr, "Memory allocation error\n");
        MPI_Abort(MPI_COMM_WORLD, 0);
    }
    return res;
}

void *handle_realloc(void *ptr, size_t size) {
    void *res = realloc(ptr, size);
    if (res == NULL) {
        fprintf(stderr, "Memory reallocation error\n");
        MPI_Abort(MPI_COMM_WORLD, 0);
    }
    return res;
}


void free_sub_matrix(SUB_MATRIX *sm) {

	for (int n=0; n<sm->localSize; n++) {
		free(sm->array[n]);
	}
	free(sm->array);
}


//----------------------------------------------------------------------------//
// utility funcs


/** 
 * copies one int array into another by value
 * args 
 *   to     = array to copy into
 *   from   = array to copy from
 *   length = length of arrays to be copied
 **/
void copy_array(int * to, int * from, int length) {
	for (int i=0; i<length; i++) {
		to[i] = from[i];
	}
}


/**
 * function that returns the smallest of 2 ints
 **/
int min(int a, int b) {
	if (a < b) {
		return a;
	}
	else {
		return b;
	}
}


/**
 * get the processor rank that holds the vth vertex. This function
 * uses arithmetic operations to decrease time complexity.
 * args
 *   v  = the vertex 
 *   vc = vertex count
 *   pc = processor count
 **/
int vertex_in_proc(int v, int vc, int pc) {
	
	int p;                      // processor n is stored in 
	int opc = vc % pc;          // overflow processor count
	int lvc = vc / pc;          // local vertex count (no overflow)
	int ovc = opc * (lvc + 1);  // overflow vertex count

	if (v < ovc) {
		// still an overflow proc
		p = v / (lvc + 1);
	}
	else {
		// past overflow procs, 
		p = opc + ((v - ovc) / lvc);
	}

	return p;
}
