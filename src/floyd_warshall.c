/**
 * CITS3402 Assigmment 2: All Pairs Shortest Path 
 * Floyd-Warshall Distributed Implementation
 *
 * File: floyd_warshall.c
 *
 * By:
 *   Ben Longbottom  | 22234771
 *   David Adams     | 22497769
 **/


#include "project.h"


/**
 * sequential implementation of flyod-warshall all pairs shortest path
 **/
int floyd_warshall(MATRIX * m) {

	int vertexCount = m->size;
	int ** dist = m->array;
	
	for (int k = 0; k < vertexCount; k++) {		

		for (int j = 0; j < vertexCount; j++) {
			
			for (int i = 0; i < vertexCount; i++) {
				if (dist[i][k] != 0 && dist[k][j] != 0) {
					if (dist[i][j] != 0) {
						dist[i][j] = min(dist[i][j], dist[i][k] + dist[k][j]);
					}
					else {
						dist[i][j] = dist[i][k] + dist[k][j];
					}
				}
			}
		}
	}
	return 0;
}


/**
 * distributed implementation of floyd-warshall all pairs shortest path.
 * args
 *   sm = sub matrix stored locally
 * ret 
 *   int: always 0
 **/
int floyd_warshall_distributed(SUB_MATRIX * sm) {

	int p;          // processor rank
	int vc;         // vertex count
	int pc;         // processor count
	int * i_dist;   // distance from i to each vertex
	int * k_dist;   // distance from k to each vertex
	vc = sm->fullSize;
	MPI_Comm_rank(MPI_COMM_WORLD, &p);
	MPI_Comm_size(MPI_COMM_WORLD, &pc);
	
	//// make derived MPI datatype for row 
	MPI_Datatype mpi_row;
	MPI_Type_contiguous(vc, MPI_INT, &mpi_row);
	MPI_Type_commit(&mpi_row);
		/**
     * makes a new MPI datatype 'mpi_row' that is vc contiguous ints. 
     * This datatype is used for distributing rows from the matrices
     * between processors.
     **/

	//// loop through k vals
	for (int k = 0; k < vc; k++) {
	
		int kl;    // k local index
		int kp;    // (row) k's processor
		kp = vertex_in_proc(k, vc, pc);
		kl = k - sm->vertexOffset;

		//// setup k_dist for broadcast (according to processor rank)
		if (kp == p) {
			k_dist = sm->array[kl];
		}
		else {
			k_dist = handle_malloc(vc * sizeof(int));
		}

		//// bcast 
		MPI_Bcast(k_dist, 1, mpi_row, kp, MPI_COMM_WORLD);
			/**
 			 * MPI_Bcast - Explanation
 			 * broadcast kth row to all processors that do not have 
 			 * the kth row locally.
 			 **/ 
			
		
		//// updates dist matrix if vertex i to j is smaller through vertex k
		for (int j = 0; j < vc; j++) {
			
			for (int i = 0; i < sm->localSize; i++) {
				i_dist = sm->array[i];
		
				// check that path i -> j -> k exists
				if (i_dist[k] != 0 && k_dist[j] != 0) {
					// check that there already is a path i -> j
					if (i_dist[j] != 0) {
						// update distance to smallest
						i_dist[j] = min(i_dist[j], i_dist[k] + k_dist[j]);
					}
					else {
						// put a valid path into dist
						i_dist[j] = i_dist[k] + k_dist[j];
					}
				}
			}
		}
		if (kp != p) {
			// free k_th row if not stored locally (ie. was recieved from bcast)
			free(k_dist);
		}
	}
	return 0;
}
