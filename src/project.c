/**
 * CITS3402 Assigmment 2: All Pairs Shortest Path 
 * Floyd-Warshall Distributed Implementation
 *
 * file: project.c
 *
 * By:
 *   Ben Longbottom  | 22234771
 *   David Adams     | 22497769
 *
 * Options:
 *   -f <filename>
 *     file: read filename as input. If filename is not given the program 
 *           will fail
 *   -o <filename>
 *     output: outputs distance matrix to filename. If filename is not 
 *             given the program will fail
 *   -t
 *     timer: display time taken to calculate the distance matrix along 
 *            with other statistics
 *   -s
 *     sequential: doesn't use MPI calls (other than MPI_Wtime())
 *   -v
 *     verbose: print input and output matrix, if not given program 
 *              will only print distance matrix
 *
 *
 * Notes:
 *   File Overwriting:
 *     supplying the same input and output file name will overwrite the file
 *   Input and Output files:
 *     Input and Output filenames must be given after corresponding flags. 
 *   Definitions:
 *     vertex - an element of a graph
 *     Processor - a 'node' within the cluster 
 *
 * Floyd-Warshall Implementation:
 *   The distribued implementation of Floyd-Warshall relies on all processors
 *   within a cluster locally holding the distance from some vertices to all other
 *   vertices within a graph. Each processor takes turns in broadcasting a single
 *   locally stored vertex (vertex k) to all other procs. All the procs then use row
 *   k to calculate the distance from its local vertices to all other vertices in the 
 *   graph through vertex k. For a graph with N vertices, this results in N 
 *   broadcasts.
 **/


/**TODO:
 * double check David's student number
 * check MPI_STATUS things
 * make new MPI_COMM if pc > vc?
 */



#include "project.h"



/**
 * Main Function
 **/
int main(int argc, char const *argv[]) {
	
	// initialise MPI and alter args (strip mpi parts out)
	MPI_Init(&argc, &argv);
	
	//// program options
	// init option vars
	int opt;
	char *file_in;
	char *file_out;
	bool write_to_file = false;
	bool distributed = true;
	bool dispTime = false;
	bool verbose = false;
	int p;
	MPI_Comm_rank(MPI_COMM_WORLD, &p); // get proc rank and store in p

	// set option vars
	while ((opt = getopt(argc, argv, ":o:f:tsv")) != -1) {
		switch(opt) {
			case 's':
				distributed = false;
				break;
			case 't':
				dispTime = true;
				break;
			case 'f':
				file_in = optarg;
				break;
			case 'o':
				write_to_file = true;
				file_out = optarg;
				break;
			case 'v':
				verbose = true;
				break;
			case ':':
				//filename was not supplied
				if (p == 0) fprintf(stderr, "test: %s: option `-%c' requires an argument\n", argv[0], optopt);
				MPI_Abort(MPI_COMM_WORLD, 0);
				break;
			default: 
				// invalid option given
				if (p == 0) fprintf(stderr, "%s: option `-%c' is invalid\n", argv[0], optopt);
				MPI_Abort(MPI_COMM_WORLD, 0);
				break;
		}
	}
	
	//// catch errors in options
	// no input file given
	if (file_in == NULL && p == 0) {
		fprintf(stderr, "%s: no input file provided\n", argv[0]);
		MPI_Abort(MPI_COMM_WORLD, 0);
	}
	// file_in doesn't exist
	else if (access(file_in, F_OK) == -1 && p == 0) {
		fprintf(stderr, "%s: input file %s does not exist\n", argv[0], file_in);
		MPI_Abort(MPI_COMM_WORLD, 0);
	}
	// read permission denied
	else if (access(file_in, R_OK) == -1 && p == 0) {
		fprintf(stderr, "%s: read permission denied for %s\n", argv[0], file_in);
		MPI_Abort(MPI_COMM_WORLD, 0);
	}
	
	// no output file name given (probably redundant) 
	if (write_to_file == true && file_out == NULL && p == 0) {
		fprintf(stderr, "%s: no output file provided\n", argv[0]);
		MPI_Abort(MPI_COMM_WORLD, 0);
	}
	// write permision denied
	else if (access(file_out, F_OK) == 0 && access(file_out, W_OK) == -1 && p == 0) {
		fprintf(stderr, "%s: write permission denied for %s\n", argv[0], file_in);
		MPI_Abort(MPI_COMM_WORLD, 0);
	}

	//// operation vars
	int pc;       // processor count
	int vc;       // vertex count
	double timer;
	// get proccessor count
	MPI_Comm_size(MPI_COMM_WORLD, &pc);
	
	if (distributed != true) {
		//// sequential code, runs when flag -s is given

		if (p == 0) {
			MATRIX m;
			read_file(&m, file_in);
			vc = m.size;

			if (verbose) {
				printf("\nog matrix:\n");
				print_matrix(&m);
			}

			// do floyd_warshall operation
		  timer = -MPI_Wtime();
			floyd_warshall(&m);
		  timer += MPI_Wtime();

			if (!write_to_file) {
				printf("\ndist mat:\n");
				print_matrix(&m);
			}
		}
	}
	else {
		//// MPI parallel code, runs by default
		// Note: all barriers are used to reduce chance of output being split up

		//// load file into sm
		SUB_MATRIX sm;
		read_file_distributed(&sm, file_in);
		vc = sm.fullSize;

		//// only print input matrix if verbose flag is set
		if (verbose) {
			if (p == 0) {
				printf("original matrix:\n");
			}
			print_matrix_distributed(&sm);
		}
	
		//// calc shortest distances
		timer = -MPI_Wtime();
		floyd_warshall_distributed(&sm);
		timer += MPI_Wtime();

		//// print to stdout if not writing to file, and print header if verbose
		// flag is set
		if (!write_to_file || verbose) {
			if (verbose) {
				if (p == 0) printf("\ndistance matrix:\n");
			}
			print_matrix_distributed(&sm);
		}

		//// write matrix to file
		if (write_to_file == true) {
			write_matrix_to_file(&sm, file_out);
		}	

		//// free sub matrix 
		free_sub_matrix(&sm);
	}
	
	//// print timer if wanted
	if (dispTime == true && p == 0) {
		printf("\nproccessor count: %d\nvertex count: %d\ntime: %f\n", pc, vc, timer);
	}

	//// finalises MPI
	MPI_Finalize();
	return 0;
}
