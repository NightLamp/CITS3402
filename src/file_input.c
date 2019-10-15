#include "project.h"

int read_file(MATRIX *m, char *filepath) {
    FILE *fp;
    int size = 0;
    fp = fopen(filepath, "rb");

    if(fp == NULL) {
        fprintf(strerror, "Unable to open file\n");
        return -1;
    }

    fread(&size, sizeof(int), 1, fp);
    printf("size of this file is: %d\n", size);

		m->size  = size;
    m->array = handle_malloc(size * sizeof(int *));

    for(int i = 0; i<size; i++) {
        int *row = (int *)handle_malloc(size * sizeof(int));
        fread(row, sizeof(int), size, fp);

				printf("row = ");
        for(int j = 0; j<size; j++) {
            printf("%d ", row[j]);
        }
        m->array[i] = row;

				printf("\tarray[%d] = ", i);
        for(int j = 0; j<size; j++) {
            printf("%d ", m->array[i][j]);
        }
				printf("\n");

    }

    return 0;
}
