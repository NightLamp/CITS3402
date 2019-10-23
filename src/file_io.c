/**
 * CITS3402 Assigmment 2: All Pairs Shortest Path 
 * Floyd-Warshall Distributed Implementation
 *
 * File: file_io.c
 *
 * By:
 *   Ben Longbottom  | 22234771
 *   David Adams     | 22497769
 **/


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
		// returns a file handle to use MPI functions on file
	
	//TODO: check for file error

	//// get proc details
	int p;		// processor rank
	int pc;		// processor count
	MPI_Comm_rank(MPI_COMM_WORLD, &p);
	MPI_Comm_size(MPI_COMM_WORLD, &pc);

	
	//// get matrix size from file
	int nc;		// node count
	// only read 1st int in file
	MPI_File_read(fh, &nc, 1, MPI_INT, MPI_STATUS_IGNORE);
	// fail if negative matrix size read
	if (nc < 0) {
		if (p == 0) fprintf(stderr, "cannot have negative matrix dimensions\n");
		exit(EXIT_FAILURE);	
	}
		/**
		 * MPI_File_read - Explanation
		 * reads 1st int from fh into nc, used independant IO because 
		 * only reading 1 int (size) and do not need syncronisation
		 **/

	//// calculate local node count and node offset
	int lnc; 	// local node count
	int no;   // node offset
	lnc = get_local_node_count(p, pc, nc);
	no = get_node_offset(p, pc, nc);


	//// make derived MPI datatype for row 
	MPI_Datatype mpi_row;
	MPI_Type_contiguous(nc, MPI_INT, &mpi_row);
	MPI_Type_commit(&mpi_row);
		/**
     * makes a new MPI datatype 'mpi_row' that is nc contiguous ints. 
     * This datatype is used for reading and writing rows from the matrices
     * in this project. 
     **/

	//// set file view with offset
	int bo;    // byte offset 
	bo = (no * nc + 1) * sizeof(int);  // +1 to skip over size
 	MPI_File_set_view(fh, bo, MPI_INT, mpi_row, "native", MPI_INFO_NULL);
		/**
		 * MPI_File_set_view - Explanation
		 * sets all processor's view of the input file to an appropriate offset
		 * such that each processor starts reading the appropriate nodes that
		 * have been 'assigned' to the processor.
		 **/

	//// read file into buffer
	int rs = nc * lnc;    // read size
	int * buffer = handle_malloc(rs*sizeof(int));
	// test read
	MPI_File_read_all(fh, buffer, lnc, mpi_row, MPI_STATUS_IGNORE);
	// close file
	MPI_File_close(&fh);
		/**
		 * MPI_File_read_all - Explanation
		 * all processors collectively read their portion of the input file
		 * into a local buffer. Collective file IO is much faster as it 
		 * reduces traffic in MPI's backend
		 **/

	//// convert buffer into sub_matrix
	sm->array = handle_malloc(lnc * sizeof(int *));
	for (int r=0; r<lnc; r++) {
		int * row = handle_malloc(nc * sizeof(int));
		copy_array(row, &(buffer[r*nc]), nc);
		sm->array[r] = row;
	}
	free(buffer);

	//// set remaining sub matrix fields appropriately
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
		/**
     * makes a new MPI datatype 'mpi_row' that is nc contiguous ints. 
     * This datatype is used for reading and writing rows from the matrices
     * in this project. 
     **/

	//// head writes matrix size to file
	if (p == 0) {
		MPI_File_write(fh, &nc, 1, MPI_INT, MPI_STATUS_IGNORE);
	}
			/**
 			 * MPI_File_write - Explanation
 			 * only the head node writes the matrix size into the file
 			 **/
			
	//// set file view with offset
	int bo;    // byte offset 
	bo = (no * nc + 1) * sizeof(int);  // +1 to skip over size of byte
 	MPI_File_set_view(fh, bo, MPI_INT, mpi_row, "native", MPI_INFO_NULL);
		/**
		 * MPI_File_set_view - Explanation
		 * sets all processor's view of the input file to an appropriate offset
		 * such that each processor starts writing its local nodes in the correct
		 * location within the file
		 **/

	//// convert sub_matrix to row major array
	int bufSiz = lnc * nc;
	int * buffer = handle_malloc(bufSiz * sizeof(int));
	// flatten the sub_matrix into a 1D row-major ordered array
	for (int r=0; r<lnc; r++) {
		copy_array(&buffer[r*nc], sm->array[r], nc);
	}
	// Above is done so that collective file IO can be used more effectively


	//// write matrix to file
	MPI_File_write_all(fh, buffer, lnc, mpi_row, MPI_STATUS_IGNORE);
		/**
		 * MPI_File_write_all - Explanation
		 * Each processor writes to the output file using collective file IO.
		 * This takes advantage of MPI's efficiency by allowing less network
		 * traffic.
		 **/

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

	int opc  = nc % pc;    // overflow processor count
	//int np  = nc - opc;    // normal processors
	int lnc = nc / pc;    // local node count (no overflow)
	int no;           // offset to be returned

	if (p < opc) {
		no = p * (lnc + 1);
	}
	else {
		no = (opc * (lnc+1)) + ((p - opc) * lnc);
	}

	return no;
}
