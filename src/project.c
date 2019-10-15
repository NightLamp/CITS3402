#include"project.h"

int main(int argc, char const *argv[])
{
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

		MATRIX m;
    read_file(&m, file1);

		printf("\nog matrix:\n");
		print_matrix(&m);
		floyd_warshall(&m);
		printf("\ndist mat:\n");
		print_matrix(&m);
		
    return 0;
}
