/**
 * CITS3402 Assigmment 2: All Pairs Shortest Path 
 * Floyd-Warshall Distributed Implementation
 *
 * File: display.c
 *
 * By:
 *   Ben Longbottom  | 22234771
 *   David Adams     | 22497769
 **/


 #include"project.h"


/**
 * print an int array
 * args
 *   arr    = arrary to print
 *   length = length of array
 **/
void print_array(int * arr, int length) {
	for (int i=0; i<length; i++) {
		printf("%d ", arr[i]);
	}
	printf("\n");
}

/**
 * print a full matrix for sequential implementation.
 * args
 *   m = matrix to print
 **/
void print_matrix(MATRIX *m) {
    int size = m->size;
    for(int i = 0; i<size; i++){
        for(int j = 0; j<size; j++){
            printf("%d ", m->array[i][j]);
        }
        printf("\n");
    }
}


/**
 * Print a full matrix when distributed.
 * This function must be called by all processors holding sub matrices
 * of the full matrix.
 * args
 *   sm = sub matrices in local processor
 **/
void print_matrix_distributed(SUB_MATRIX *sm) {

	int vc;  // vertex count
	int p;   // processor (rank)
	int pc;  // processor  count

	vc = sm->fullSize;
	MPI_Comm_rank(MPI_COMM_WORLD, &p);
	MPI_Comm_size(MPI_COMM_WORLD, &pc);

	for (int i=0; i<pc; i++) {
		MPI_Barrier(MPI_COMM_WORLD); // attempt to keep vertex rows printed in order
		if (p == i) {
			for (int n=0; n<sm->localSize; n++) {
				print_array(sm->array[n], vc);
			}
		}
	}
}



/**
 * prints sub_matrix (depreciated)
 * */
void print_sub_matrix(SUB_MATRIX *m) {
    int vertexCount = m->fullSize;
		int localCount = m->localSize;

    for(int i = 0; i<localCount; i++){
        for(int j = 0; j<vertexCount; j++){
            printf("%d ", m->array[i][j]);
        }
        printf("\n");
    }
}


