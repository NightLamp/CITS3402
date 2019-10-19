#include"project.h"

void *handle_malloc(size_t size) {
    void *res = malloc(size);
    if (res == NULL) {
        fprintf(stderr, "Memory allocation error\n");
        exit(EXIT_FAILURE);
    }
    return res;
}

void *handle_realloc(void *ptr, size_t size) {
    void *res = realloc(ptr, size);
    if (res == NULL) {
        fprintf(stderr, "Memory reallocation error\n");
        exit(EXIT_FAILURE);
    }
    return res;
}


void copy_array(int * to, int * from, int length) {
	for (int i=0; i<length; i++) {
		to[i] = from[i];
	}
}
