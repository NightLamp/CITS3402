//TODO:
// make error printing only by head node



#include"project.h"

int main(int argc, char const *argv[])
{
    MPI_Init(&argc, &argv);
    int opt;
    char *file_in;
    char *file_out;
    bool write_to_file = false;
    bool distributed = false;
		bool dispTime = false;
    

    while((opt = getopt(argc, argv, ":o:ftd")) != -1) {
        switch(opt) {
            case 'd':
                distributed = true;
            case 't':
                dispTime = true;
            case 'f':
                file_in = optarg;
            case 'o':
                write_to_file = true;
                file_out = optarg;
                break;
            case ':':
                //filename was not supplied
                fprintf(stderr, "%s: option `-%c' requires an argument\n", argv[0], optopt);
                exit(EXIT_FAILURE);
                break;
            default: 
                fprintf(stderr, "%s: option `-%c' is invalid: ignored\n", argv[0], optopt);
                exit(EXIT_FAILURE);
                break;
        }
    }

    if(file_in == NULL){
        fprintf(stderr, "%s: no file provided\n", argv[0]);
        exit(EXIT_FAILURE);
    }


	int p;
	int pc;
	int nc;
	double time;

	MPI_Comm_rank(MPI_COMM_WORLD, &p);
	MPI_Comm_size(MPI_COMM_WORLD, &pc);

	// depreciated, there for reference currently
	if (distributed != true) {
		if (p == 0) {
			MATRIX m;
			read_file(&m, file_in);

			printf("\nog matrix:\n");
			print_matrix(&m);

		  time = -MPI_Wtime();
			floyd_warshall(&m);
		  time += MPI_Wtime();

			printf("\ndist mat:\n");
			print_matrix(&m);

			nc = m.size;
		}
	}
	else {
		//// load file into sm
		SUB_MATRIX sm;
		read_file_distributed(&sm, file_in);
		nc = sm.fullSize;

		//// print original matrix
		MPI_Barrier(MPI_COMM_WORLD);
		if (p == 0) {
			printf("original matrix:\n");
		}
		print_matrix_distributed(&sm);
	
		//// calc shortest distances
		time = -MPI_Wtime();
		floyd_warshall_distributed(&sm);
		time += MPI_Wtime();

		//// print result mat
		MPI_Barrier(MPI_COMM_WORLD);
		if (p == 0) {
			printf("\ndistance matrix:\n");
		}
		print_matrix_distributed(&sm);

		//// write matrix to file
		if (write_to_file == true) {
			MPI_Barrier(MPI_COMM_WORLD);
			write_matrix_to_file(&sm, file_out);
		}	

		free_sub_matrix(&sm);
	}
	
	//// print timer if wanted
	MPI_Barrier(MPI_COMM_WORLD);
	if (dispTime == true && p == 0) {
		printf("proccessor count: %d\nnode count: %d\ntime: %f\n", pc, nc, time);

	}

	MPI_Finalize();
	return 0;
}
