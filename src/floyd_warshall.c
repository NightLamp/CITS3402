#include "project.h"



int floyd_warshall(MATRIX * m) {

	int nodeCount = m->size;
	int ** dist = m->array;
	

	for (int k = 0; k < nodeCount; k++) {
		// 	if (k == local) 
		// 		broadcast to others
		// 	else 
		// 		recieve bcast
		
		for (int j = 0; j < nodeCount; j++) {
		// maybe not bcast here?
			// 	if (k == local) 
			// 		broadcast to others
			// 	else 
			// 		recieve bcast
			
			for (int i = 0; i < nodeCount; i++) {
				if ((dist[i][j] != 0 && dist[i][k] != 0 && dist[k][j]) != 0) {
					dist[i][j] = min(dist[i][j], dist[i][k] + dist[k][j]);
				}
			}
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
