#include "project.h"



int floyd_warshall(MATRIX * m) {

	int nodeCount = m->size;
	int ** dist = m->array;
	
	for (int k = 0; k < nodeCount; k++) {		

		for (int j = 0; j < nodeCount; j++) {
			
			for (int i = 0; i < nodeCount; i++) {
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

int floyd_warshall_distributed(SUB_MATRIX * sm) {


	int p;          // processor rank
	int nc;         // node count
	int pc;         // processor count
	int * k_dist;   // distance from k to each node
	int ** dist;    // distance matrix alias
	
	nc = sm->fullSize;
	MPI_Comm_rank(MPI_COMM_WORLD, &p);
	MPI_Comm_size(MPI_COMM_WORLD, &pc);
	dist = sm->array;

	//// loop through k vals
	for (int k = 0; k < nc; k++) {
	
		int kl;    // k local index
		int kp;    // procssor holding k
		kp = node_in_proc(k, nc, pc);
		kl = k - sm->nodeOffset + 1;

		//// set k_dist if node k is local
		if (kp == p) {
			k_dist = sm->array[kl];
		}
		else {
			k_dist = handle_malloc(nc * sizeof(int));
		}

		//// bcast 
		// barrier here?
		MPI_Bcast(k_dist, nc, MPI_INT, kp, MPI_COMM_WORLD);
		
		//// updates if node i to j is smaller through node k
		for (int j = 0; j < nc; j++) {
			
			for (int i = 0; i < sm->localSize; i++) {
				// i_dist = sm->array[i];
		
				// check that path i -> j -> k exists
				if (dist[i][k] != 0 && k_dist[j] != 0) {
					// check that there already is a path i -> j
					if (dist[i][j] != 0) {
						// update distance to smallest
						dist[i][j] = min(dist[i][j], dist[i][k] + k_dist[j]);
					}
					else {
						// put a valid path into dist
						dist[i][j] = dist[i][k] + k_dist[j];
					}
				}
			}
		}
		if (kp != p) {
			// free read buffer if not stored in sub_matrix
			free(k_dist);
		}
	}
	return 0;
}


int min(int a, int b) {
	if (a < b) {
		return a;
	}
	else {
		return b;
	}
}
