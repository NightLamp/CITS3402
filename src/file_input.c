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

    m->array = handle_malloc(size * sizeof(int *));

    for(int i = 0; i<size; i++) {
        int *row = (int *)handle_malloc(size * sizeof(int));
        fread(row, sizeof(int), size, fp);
        for(int i = 0; i<size; i++) {
            printf("%d ", row[i]);
        }
        m->array[i] = row;
    }

    return 0;
}