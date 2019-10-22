#include "project.h"

/**
 * reads file into SUB_MATRIX structs in individual processors.
 * args
 *   m        = sub matrix pointer 
 *   filepath = path to input file
 * ret
 *   int: always 0
 **/
int read_file_distributed(SUB_MATRIX *sm, char *filepath) {

	//// open file for reading distribution
	MPI_File fh;
	MPI_File_open(MPI_COMM_WORLD, filepath, MPI_MODE_RDONLY, MPI_INFO_NULL, &fh);
	
	//TODO: check for file error

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
	MPI_Datatype mpi_row;
	MPI_Type_contiguous(nc, MPI_INT, &mpi_row);
	MPI_Type_commit(&mpi_row);

	//// set file view with offset
	int bo;    // byte offset 
	bo = (no * nc + 1) * sizeof(int);
 	MPI_File_set_view(fh, bo, MPI_INT, mpi_row, "native", MPI_INFO_NULL);

	//// read file into buffer
	int rs = nc * lnc;    // read size
	int * buffer = handle_malloc(rs*sizeof(int));
	// test read
	MPI_File_read_all(fh, buffer, lnc, mpi_row, MPI_STATUS_IGNORE);

	MPI_File_close(&fh);

	//// convert buffer into sub_matrix
	sm->array = handle_malloc(lnc * sizeof(int *));
	for (int r=0; r<lnc; r++) {
		int * row = handle_malloc(nc * sizeof(int));
		copy_array(row, &(buffer[r*nc]), nc);
		sm->array[r] = row;
	}
	free(buffer);

	sm->fullSize = nc;
	sm->localSize = lnc;
	sm->nodeOffset = no;
	
	return 0;
}


/**
 * write the distributd matrix to a single file on the head node
 * args
 *   sm       = sub matrix that stores local nodes of matrix 
 *   filepath = file name of file to be written to
 **/
int write_matrix_to_file(SUB_MATRIX *sm, char *filepath) {
	
	//// open file for reading distribution
	MPI_File fh;
	MPI_File_open(MPI_COMM_WORLD, filepath, MPI_MODE_WRONLY | MPI_MODE_CREATE, MPI_INFO_NULL, &fh);
	
	//TODO: check for file error

	//// get proc details
	int p;		// processor rank
	int pc;		// processor count
	MPI_Comm_rank(MPI_COMM_WORLD, &p);
	MPI_Comm_size(MPI_COMM_WORLD, &pc);

	//// set vars from sub matrix
	int no;  // node offset
	int nc;  // node count
	int lnc; //local node count
	no = sm->nodeOffset;
	nc = sm->fullSize;	
	lnc = sm->localSize;

	//// make datatype for writing 
	MPI_Datatype mpi_row;
	MPI_Type_contiguous(nc, MPI_INT, &mpi_row);
	MPI_Type_commit(&mpi_row);

	//// head writes matrix size to file
	if (p == 0) {
		MPI_File_write(fh, &nc, 1, MPI_INT, MPI_STATUS_IGNORE);
	}
			
	//// set file view with offset
	int bo;    // byte offset 
	bo = (no * nc + 1) * sizeof(int);  // +1 to skip over size of byte
 	MPI_File_set_view(fh, bo, MPI_INT, mpi_row, "native", MPI_INFO_NULL);

	//// convert sub mat to row major array
	int bufSiz = lnc * nc;
	int * buffer = handle_malloc(bufSiz * sizeof(int));
	for (int r=0; r<lnc; r++) {
		copy_array(&buffer[r*nc], sm->array[r], nc);
	}

	//// write matrix to file
	MPI_File_write_all(fh, buffer, lnc, mpi_row, MPI_STATUS_IGNORE);

	//// frees and closes
	free(buffer);
	MPI_File_close(&fh);

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
 * returns how many nodes are before the current proc, ie. the node offset
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

	return no;
}
