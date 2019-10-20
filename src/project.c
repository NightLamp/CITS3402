#include"project.h"

int main(int argc, char const *argv[])
{
    MPI_Init(&argc, &argv);
    int opt;
    char *file1;
    while((opt = getopt(argc, argv, ":f:n")) != -1) {
        switch(opt) {
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
	MPI_Comm_rank(MPI_COMM_WORLD, &p);
	int distributed = false;

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
		SUB_MATRIX sm;
		read_file_distributed(&sm, file1);

		MPI_Barrier(MPI_COMM_WORLD);
		if (p == 0) {
			printf("sub_matrix:\n");
		}
		print_matrix_distributed(&sm);
	
		// calc shortest distances
		floyd_warshall_distributed(&sm);

		MPI_Barrier(MPI_COMM_WORLD);
		if (p == 0) {
			printf("\nresult mat:\n");
		}
		print_matrix_distributed(&sm);
	}

    MPI_Finalize();
    return 0;
}
