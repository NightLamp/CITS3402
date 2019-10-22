#include"project.h"



//----------------------------------------------------------------------------//
// memory funcs

void *handle_malloc(size_t size) {
    void *res = malloc(size);
    if (res == NULL) {
        fprintf(stderr, "Memory allocation error\n");
        exit(EXIT_FAILURE);
    }
    return res;
}

void *handle_realloc(void *ptr, size_t size) {
    void *res = realloc(ptr, size);
    if (res == NULL) {
        fprintf(stderr, "Memory reallocation error\n");
        exit(EXIT_FAILURE);
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
 * function that returns the smallest of the 2 ints
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
 * get the processor rank that holds the nth node
 * args
 *   n  = the node 
 *   nc = node count
 *   pc = processor count
 **/
int node_in_proc(int n, int nc, int pc) {
	
	int p;                      // processor n is stored in 
	int opc = nc % pc;          // overflow processor count
	int lnc = nc / pc;          // local node count (no overflow)
	int onc = opc * (lnc + 1);  // overflow node count

	if (n < onc) {
		// still an overflow proc
		p = n / (lnc + 1);
	}
	else {
		// past overflow procs, 
		p = opc + ((n - onc) / lnc);
	}

	return p;
}
