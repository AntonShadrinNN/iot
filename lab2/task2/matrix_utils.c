#include <stdio.h>
#include <stdlib.h>
#include "matrix_utils.h"

#define SIZE 4096

void fillMatrix(int **A) {
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j <= i; j++) {
            if (i == j) {
                A[i][j] = i + SIZE;
            } else {
                A[i][j] = i;
                A[j][i] = i;
            }
        }
    }
}

void allocMatrix(int ***mat, int size_1) {
    // Allocate rows*cols contiguous items
    int *p = (int *)malloc(sizeof(int *) * size_1 * size_1);

    // Allocate row pointers
    *mat = (int **)malloc(size_1 * sizeof(int *));

    // Set up the pointers into the contiguous memory
    for (int i = 0; i < size_1; i++) {
        (*mat)[i] = &(p[i * size_1]);
    }
}

int freeMatrix(int ***mat) {
    free(&((*mat)[0][0]));
    free(*mat);
    return 0;
}

void multiplyMatrices(int **a, int **b, int size_1, int ***c) {
    for (int i = 0; i < size_1; ++i) {
        for (int j = 0; j < size_1; ++j) {
            int val = 0;
            for (int k = 0; k < size_1; ++k) {
                val += a[i][k] * b[k][j];
            }
            (*c)[i][j] = val;
        }
    }
}