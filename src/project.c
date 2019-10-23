/**
 * CITS3402 Assigmment 2: All Pairs Shortest Path 
 * Floyd-Warshall Distributed Implementation
 *
 * By:
 *   Ben Longbottom  | 22234771
 *   David Adams     | 224.....
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
 **/


/**TODO:
 * add David's student number
 * explain MPI calls
 * make error printing only by head node (other files)
 * check MPI_STATUS things
 * ensure printing doesn't get cut in half
 */



#include"project.h"



/**
 * Main Function
 **/
int main(int argc, char const *argv[]) {
	
	// initialise MPI and alter args (strip mpi parts out)
	MPI_Init(&argc, &argv);
	
	//// process options
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
				exit(EXIT_FAILURE);
				break;
			default: 
				// invalid option given
				if (p == 0) fprintf(stderr, "%s: option `-%c' is invalid\n", argv[0], optopt);
				exit(EXIT_FAILURE);
				break;
		}
	}
	
	//// catch errors in options
	// no input file given
	if (file_in == NULL) {
		fprintf(stderr, "%s: no input file provided\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	// no output file name given
	if (write_to_file == true && file_out == NULL) {
		fprintf(stderr, "%s: no output file provided\n", argv[0]);
		exit(EXIT_FAILURE);
	}


	//// operation vars
	int pc;       // processor count
	int nc;       // node count
	double timer;
	// get proccessor count
	MPI_Comm_size(MPI_COMM_WORLD, &pc);
	
	if (distributed != true) {
		//// sequential code, runs when flag -s is given

		if (p == 0) {
			MATRIX m;
			read_file(&m, file_in);
			nc = m.size;

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
		nc = sm.fullSize;

		//// only print input matrix if verbose flag is set
		if (verbose) {
			MPI_Barrier(MPI_COMM_WORLD);    
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
				MPI_Barrier(MPI_COMM_WORLD);
				if (p == 0) printf("\ndistance matrix:\n");
			}
			print_matrix_distributed(&sm);
		}

		//// write matrix to file
		if (write_to_file == true) {
			MPI_Barrier(MPI_COMM_WORLD);
			write_matrix_to_file(&sm, file_out);
		}	

		//// free sub matrix 
		free_sub_matrix(&sm);
	}
	
	//// print timer if wanted
	MPI_Barrier(MPI_COMM_WORLD);
	if (dispTime == true && p == 0) {
		printf("\nproccessor count: %d\nnode count: %d\ntime: %f\n", pc, nc, timer);
	}

	//// finalises MPI
	MPI_Finalize();
	return 0;
}
