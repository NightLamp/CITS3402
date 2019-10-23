#include"project.h"


void debug() {
    printf("made it here\n");
}

void print_matrix(MATRIX *m) {
    int size = m->size;
    for(int i = 0; i<size; i++){
        for(int j = 0; j<size; j++){
            printf("%d ", m->array[i][j]);
        }
        printf("\n");
    }
}


void print_matrix_distributed(SUB_MATRIX *sm) {

	int nc; 
	int p;
	int pc;

	nc = sm->fullSize;
	MPI_Comm_rank(MPI_COMM_WORLD, &p);
	MPI_Comm_size(MPI_COMM_WORLD, &pc);

	for (int i=0; i<pc; i++) {
		MPI_Barrier(MPI_COMM_WORLD); // attempt to keep nodes printed in order
		if (p == i) {
			for (int n=0; n<sm->localSize; n++) {
				print_array(sm->array[n], nc);
			}
		}
	}
}



/**
 * prints sub_matrix (depreciated)
 * */
void print_sub_matrix(SUB_MATRIX *m) {
    int nodeCount = m->fullSize;
		int localCount = m->localSize;

    for(int i = 0; i<localCount; i++){
        for(int j = 0; j<nodeCount; j++){
            printf("%d ", m->array[i][j]);
        }
        printf("\n");
    }
}


void print_array(int * arr, int length) {
	for (int i=0; i<length; i++) {
		printf("%d ", arr[i]);
	}
	printf("\n");
}
