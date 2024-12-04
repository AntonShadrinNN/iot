#ifndef MATRIX_UTILS_H
#define MATRIX_UTILS_H

void fillMatrix(int **A);
void allocMatrix(int ***mat, int size_1);
int freeMatrix(int ***mat);
void multiplyMatrices(int **a, int **b, int size_1, int ***c);

#endif // MATRIX_UTILS_H
