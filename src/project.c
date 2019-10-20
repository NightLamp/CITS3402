#include"project.h"

int main(int argc, char const *argv[])
{
    MPI_Init(&argc, &argv);
    int opt;
    char *file1;
    bool distributed = false;
		bool dispTime = false;

    while((opt = getopt(argc, argv, ":f:nt")) != -1) {
        switch(opt) {
            case 't':
                dispTime = true;
            case 'f':
                file1 = optarg;
                break;
            case ':':
                //filename was not supplied
                fprintf(stderr, "%s: option `-%c' requires an argument\n", argv[0], optopt);
                break;
            default: 
                fprintf(stderr, "%s: option `-%c' is invalid: ignored\n", argv[0], optopt);
                break;
        }
    }

    if(file1 == NULL){
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
	if (distributed == true) {
		if (p == 0) {
			MATRIX m;
			read_file(&m, file1);

			printf("\nog matrix:\n");
			print_matrix(&m);
			floyd_warshall(&m);
			printf("\ndist mat:\n");
			print_matrix(&m);
		}
	}
	else {
		//// load file into sm
		SUB_MATRIX sm;
		read_file_distributed(&sm, file1);
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

		if (dispTime == true) {
			printf("proccessor count: %d\nnode count: %d\ntime: %f\n", pc, nc, time);
		}
	}

    MPI_Finalize();
    return 0;
}
