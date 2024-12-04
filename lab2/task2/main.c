#include <math.h>
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "matrix_utils.h"

#define SIZE 4096

int main(int argc, char *argv[]) {
    // Pointers to the main matrices
    int **matrixA = NULL, **matrixB = NULL, **matrixC = NULL;

    // total number of elements in matrices
    int totalElements = SIZE * SIZE;

    // Local blocks of matrices for each process
    int **localMatrixA = NULL, **localMatrixB = NULL, **localMatrixC = NULL;

    MPI_Comm processGrid;                     // Communicator for creating the process grid
    int totalProcesses;                       // Total number of processes
    int gridDimension;                        // Number of processes along one side of the grid
    int blockSize;                            // Size of each block for a process
    int broadcastParams[2], processCoords[2]; // Broadcast data and process coordinates

    // Neighbor process ranks
    int leftNeighbor, rightNeighbor, topNeighbor, bottomNeighbor;

    // Timing variables
    clock_t start, end;

    // Initialize MPI
    start = clock();
    MPI_Init(&argc, &argv);

    // Get total number of processes
    MPI_Comm_size(MPI_COMM_WORLD, &totalProcesses);

    // Get rank of the current process
    int processRank;
    MPI_Comm_rank(MPI_COMM_WORLD, &processRank);

    // Main process initializes the original matrices
    if (processRank == 0) {
        gridDimension = (int)sqrt(totalProcesses);    // Size of the process grid
        blockSize = SIZE / (int)sqrt(totalProcesses); // Size of each block for a process

        // Allocate memory for matrices A, B, and C
        allocMatrix(&matrixA, SIZE);
        allocMatrix(&matrixB, SIZE);
        allocMatrix(&matrixC, SIZE);

        /// Fill matrices A and B with data
        fillMatrix(matrixA);
        fillMatrix(matrixB);

        // Prepare data for broadcasting grid and block parameters
        broadcastParams[0] = gridDimension;
        broadcastParams[1] = blockSize;
    }

    // Broadcast grid parameters and block size to all processes
    MPI_Bcast(&broadcastParams, 4, MPI_INT, 0, MPI_COMM_WORLD);
    gridDimension = broadcastParams[0];
    blockSize = broadcastParams[1];

    // Create a Cartesian grid for processes
    int gridDimensions[2] = {gridDimension, gridDimension}; // Grid dimensions
    int gridPeriods[2] = {1, 1};                            // Enable periodic boundaries
    int allowReorder = 1;                                   // Allow process reordering
    MPI_Cart_create(MPI_COMM_WORLD, 2, gridDimensions, gridPeriods, allowReorder, &processGrid);

    // Define MPI data type for transmitting submatrices
    int originalMatSize[2] = {SIZE, SIZE};
    int subMatSize[2] = {blockSize, blockSize};
    int startInd[2] = {0, 0};
    MPI_Datatype type, subarrtype;
    MPI_Type_create_subarray(2, originalMatSize, subMatSize, startInd, MPI_ORDER_C, MPI_INT, &type);
    MPI_Type_create_resized(type, 0, blockSize * sizeof(int), &subarrtype);
    MPI_Type_commit(&subarrtype);

    // Global matrix pointers
    int *globalptrA = NULL;
    int *globalptrB = NULL;
    int *globalptrC = NULL;
    if (processRank == 0) {
        globalptrA = &(matrixA[0][0]);
        globalptrB = &(matrixB[0][0]);
        globalptrC = &(matrixC[0][0]);
    }

    // Data for scattering submatrices
    int *sendCounts = (int *)malloc(sizeof(int) * totalProcesses);    // Number of submatrices for each process
    int *displacements = (int *)malloc(sizeof(int) * totalProcesses); // Displacements for submatrices

    if (processRank == 0) {
        for (int i = 0; i < totalProcesses; i++) {
            sendCounts[i] = 1; // One submatrix per process
        }

        // Compute displacements for scattering
        int displacement = 0;
        for (int i = 0; i < gridDimension; i++) {
            for (int j = 0; j < gridDimension; j++) {
                displacements[i * gridDimension + j] = displacement;
                displacement++;
            }
            displacement += (blockSize - 1) * gridDimension;
        }
    }

    // Allocate memory for local submatrices
    allocMatrix(&localMatrixA, blockSize);
    allocMatrix(&localMatrixB, blockSize);

    // Scatter submatrices A and B to processes
    MPI_Scatterv(globalptrA, sendCounts, displacements, subarrtype, &(localMatrixA[0][0]),
                 totalElements / (totalProcesses), MPI_INT,
                 0, MPI_COMM_WORLD);
    MPI_Scatterv(globalptrB, sendCounts, displacements, subarrtype, &(localMatrixB[0][0]),
                 totalElements / (totalProcesses), MPI_INT,
                 0, MPI_COMM_WORLD);

    // Initialize local matrix C
    allocMatrix(&localMatrixC, blockSize);

    // Init C
    for (int i = 0; i < blockSize; i++) {
        for (int j = 0; j < blockSize; j++) {
            localMatrixC[i][j] = 0; // Initialize to zero
        }
    }

    // Determine process coordinates in the grid
    MPI_Cart_coords(processGrid, processRank, 2, processCoords);

    // Perform initial shifts for Cannon's algorithm
    MPI_Cart_shift(processGrid, 1, processCoords[0], &leftNeighbor, &rightNeighbor);
    MPI_Sendrecv_replace(&(localMatrixA[0][0]), blockSize * blockSize, MPI_INT, leftNeighbor, 1, rightNeighbor, 1, processGrid, MPI_STATUS_IGNORE);

    MPI_Cart_shift(processGrid, 0, processCoords[1], &topNeighbor, &bottomNeighbor);
    MPI_Sendrecv_replace(&(localMatrixB[0][0]), blockSize * blockSize, MPI_INT, topNeighbor, 1, bottomNeighbor, 1, processGrid, MPI_STATUS_IGNORE);

    // Cannon's matrix multiplication algorithm
    int **multiplyRes = NULL;
    allocMatrix(&multiplyRes, blockSize);

    for (int k = 0; k < gridDimension; k++) {
        multiplyMatrices(localMatrixA, localMatrixB, blockSize, &multiplyRes);

        for (int i = 0; i < blockSize; i++) {
            for (int j = 0; j < blockSize; j++) {
                localMatrixC[i][j] += multiplyRes[i][j];
            }
        }

        // left Shift matrix A
        MPI_Cart_shift(processGrid, 1, 1, &leftNeighbor, &rightNeighbor);
        MPI_Sendrecv_replace(&(localMatrixA[0][0]), blockSize * blockSize, MPI_INT, leftNeighbor, 1, rightNeighbor, 1, processGrid, MPI_STATUS_IGNORE);

        // Up Shift matrix B
        MPI_Cart_shift(processGrid, 0, 1, &topNeighbor, &bottomNeighbor);
        MPI_Sendrecv_replace(&(localMatrixB[0][0]), blockSize * blockSize, MPI_INT, topNeighbor, 1, bottomNeighbor, 1, processGrid, MPI_STATUS_IGNORE);
    }

    // Gather results into the global matrix C
    MPI_Gatherv(&(localMatrixC[0][0]), totalElements / totalProcesses, MPI_INT,
                globalptrC, sendCounts, displacements, subarrtype,
                0, MPI_COMM_WORLD);

    // Free local matrices
    freeMatrix(&localMatrixC);
    freeMatrix(&multiplyRes);

    // Finalize MPI and calculate execution time
    MPI_Finalize();
    end = clock();

    // Save results
    char fileName[20];
    sprintf(fileName, "%d/%d", SIZE, totalProcesses);

    FILE *output = fopen(fileName, "w");
    fprintf(output, "%lf", (double)(end - start) / CLOCKS_PER_SEC);
    fclose(output);

    return 0;
}