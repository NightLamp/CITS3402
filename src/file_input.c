#include "project.h"

int read_file(MATRIX *m, char *filepath) {
    FILE *fp;
    int size = 0;
    fp = fopen(filepath, "rb");

    if(fp == NULL) {
        fprintf(strerror, "Unable to open file\n");
        return -1;
    }

    fread(&size, sizeof(int), 1, fp);
    printf("size of this file is: %d\n", size);

		m->size  = size;
    m->array = handle_malloc(size * sizeof(int *));

    for(int i = 0; i<size; i++) {
        int *row = (int *)handle_malloc(size * sizeof(int));
        fread(row, sizeof(int), size, fp);

				printf("row = ");
        for(int j = 0; j<size; j++) {
            printf("%d ", row[j]);
        }
        m->array[i] = row;

				printf("\tarray[%d] = ", i);
        for(int j = 0; j<size; j++) {
            printf("%d ", m->array[i][j]);
        }
				printf("\n");

    }

    return 0;
}


int read_file_distributed(SUB_MATRIX *m, char *filepath) {

	//// open file for reading distribution
	MPI_File fh;
	MPI_File_open(MPI_COMM_WORLD, filepath, MPI_MODE_RDONLY, MPI_INFO_NULL, &fh);
	

	//// get proc details
	int p;		// processor rank
	int pc;		// processor count
	MPI_Comm_rank(MPI_COMM_WORLD, &p);
	MPI_Comm_size(MPI_COMM_WORLD, &pc);

	
	//// get matrix size from file
	int nc;		// node count
	// only read 1st int in file
	MPI_File_read_all(fh, &nc, 1, MPI_INT, MPI_STATUS_IGNORE);
	if (nc < 0) {
		fprintf(stderr, "cannot have negative matrix dimensions\n");
		exit(EXIT_FAILURE);	
	}
	//// calculate local node count
	int lnc; 	// local node count
	lnc = get_local_node_count(p, pc, nc);

	//// calculate read offset 
	int no;   // node offset
	no = get_node_offset(p, pc, nc);

	//// make datatype for reading 
	MPI_Datatype readBuf;
	MPI_Type_contiguous(nc, MPI_INT, &readBuf);
	MPI_Type_commit(&readBuf);

	//// set file view with offset
	int bo;    // binary offset 
	bo = ((no - 1) * nc + 1) * sizeof(int);
 	MPI_File_set_view(fh, bo, MPI_INT, readBuf, "native", MPI_INFO_NULL);

	//// read file into buffer
	int rs = nc * lnc;    // read size
	int * buffer = handle_malloc(rs*sizeof(int));
	// test read
	MPI_File_read_all(fh, buffer, lnc, readBuf, MPI_STATUS_IGNORE);

	MPI_File_close(&fh);

	//// convert buffer into sub_matrix
	m->array = handle_malloc(lnc * sizeof(int *));
	for (int r=0; r<lnc; r++) {
		int * row = handle_malloc(nc * sizeof(int));
		copy_array(row, &(buffer[r*nc]), nc);
		m->array[r] = row;
	}
	free(buffer);

	m->fullSize = nc;
	m->localSize = lnc;
	m->nodeOffset = no;
	
	return 0;
}


/**
 * if there is remainder 'r', first r procs have +1 nodes
 * args
 *   p  = processor rank
 *   pc = processor count
 *   nc = node count 
 * ret
 *   number of nodes stored in local proc
 **/
int get_local_node_count(int p, int pc, int nc) { 
	if (p < (nc % pc)) {
		return nc / pc + 1;
	}
	else {
		return nc / pc;
	}
}


/**
 * gets the node offset of sub_mattrix
 * args
 *   p  = processor rank
 *   pc = processor count
 *   nc = node count 
 * ret
 *   node offset
 **/
int get_node_offset(int p, int pc, int nc) {

	int op  = nc % pc;    // overflow processors
	//int np  = nc - op;    // normal processors
	int lnc = nc / pc;    // local node count (no overflow)
	int no;           // offset to be returned

	if (p < op) {
		no = p * (lnc + 1);
	}
	else {
		no = (op * (lnc+1)) + ((p - op) * lnc);
	}

	return no + 1;
}
