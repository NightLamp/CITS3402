/**
 * CITS3402 Assigmment 2: All Pairs Shortest Path 
 * Floyd-Warshall Distributed Implementation
 *
 * File: file_io_sequential.c
 *
 * By:
 *   Ben Longbottom  | 22234771
 *   David Adams     | 22497769
 **/


#include "project.h"



/**
 * reads file into MATRIX struct for later use.
 * args
 *   m        = matrix struct to read file into
 *   filepath = path to input file
 * ret
 *   int: always 0
 **/
int read_file(MATRIX *m, char *filepath) {
    FILE *fp;
    int size = 0;
    fp = fopen(filepath, "rb");

    if(fp == NULL) {
        fprintf(stderr, "Unable to open file\n");
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
