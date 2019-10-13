#include"project.h"

void debug() {
    printf("made it here\n");
}

void print_matrix(MATRIX *m) {
    int size = m->size;
    for(int i = 0; i<size; i++){
        for(int j = 0; j<size; j++){
            printf("%d ", m->array[i][j]);
        }
        printf("\n");
    }
}