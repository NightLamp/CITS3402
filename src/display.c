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
 * of the full matrix. This function cannot print out of order.
 * args
 *   sm = sub matrices in local processor
 **/
void print_matrix_distributed(SUB_MATRIX *sm) {

	int vc;  // vertex count
	int p;   // processor (rank)
	int pc;  // processor  count
	int tag = 1;

	vc = sm->fullSize;
	MPI_Comm_rank(MPI_COMM_WORLD, &p);
	MPI_Comm_size(MPI_COMM_WORLD, &pc);

	//// make derived MPI datatype for row 
	MPI_Datatype mpi_row;
	MPI_Type_contiguous(vc, MPI_INT, &mpi_row);
	MPI_Type_commit(&mpi_row);
		/**
     * makes a new MPI datatype 'mpi_row' that is vc contiguous ints. 
     * This datatype is used for reading and writing rows from the matrices
     * in this project. 
     **/

	for (int v=0; v<vc; v++) {
		int vp;  // vertex's processor	
		vp = vertex_in_proc(v, vc, pc);

		// print local vertices in head proc
		if (vp == 0 && p == 0) {
			print_array(sm->array[v], vc);
		}
		// send vertex v to head proc
		else if (p == vp && p != 0) {
			// send mpi_row to head proc
			int lvi;   // local vertex index
			lvi = v - sm->vertexOffset;
			MPI_Send(sm->array[lvi], 1, mpi_row, 0, tag, MPI_COMM_WORLD);
				/**
 				 * MPI_Send - Explanation
 				 * all non-head processors send the vth vertex to the head processor to
 				 * print. Point to Point communication is used as only the head 
 				 * node needs to recieve the vth vertex.
 				 **/
		}
		// recv vertex from other nodes and print it
		else if (p == 0) {
			// recv mpi_row 
			int * buffer = handle_malloc(vc * sizeof(int));
			MPI_Recv(buffer, 1, mpi_row, vp, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
				/**
 				 * MPI_Recv - Explanation
 				 * the head processor individually recieves each vertex row one at a time 
 				 **/
			// print row
			print_array(buffer, vc);
			free(buffer);
		}
	}
	if (p ==0) printf("\n");
}



/**
 * Print a full matrix when distributed.
 * This function must be called by all processors holding sub matrices
 * of the full matrix.
 * args
 *   sm = sub matrices in local processor
 **/
void old_print_matrix_distributed(SUB_MATRIX *sm) {

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


