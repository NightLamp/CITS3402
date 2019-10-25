#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#define DEF_SIZE 128

int main(int argc, char *argv[]){
    int size = atoi(argv[1]);
    if(size == 0){
        size = DEF_SIZE;
    }

    srand(time(0));

    FILE *fout = fopen("file.in", "w+");
    FILE *tout = fopen("file.txt", "w+");
    if(!fout || !tout){
        fprintf(stderr, "Error opening file\n");
        exit(EXIT_FAILURE);
    }

    int *buffer;
    buffer = calloc(size * size + 1, sizeof(int));
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            int loc = i*size + j + 1;
            if(i==j){
                buffer[loc] = 0;
            } else {
                buffer[loc] = rand() % DEF_SIZE;
            }
        }
    }
    buffer[0] = size;
    fprintf(tout, "%d\n", buffer[0]);
    for(int i = 0; i < size; ++i){
        for (int j = 0; j < size; ++j) {
            int loc = i * size + j + 1;
            fprintf(tout, "%d ", buffer[loc]);
        }
        fprintf(tout, "\n");
    }

    printf("Finished %d\n", size);
    fwrite(buffer, sizeof(int), size * size + 1, fout);
    free(buffer);
    fclose(fout);
    fclose(tout);
    return 0;
}