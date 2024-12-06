#include <iostream>
#include <mpi.h>
#include <string>
#include <vector>
#include <random>
#include <algorithm>

// Calculate matrix multiplication
void matrixVectorMultiplyByBlocks(const float* matrix, const float* vector, float* result, size_t numCols, size_t startRow, size_t endRow, size_t startCol, size_t endCol) {
    for (size_t i = startRow; i < endRow; ++i) {
        for (size_t j = startCol; j < endCol; ++j) {
            result[i] += matrix[i * numCols + j] * vector[j];
        }
    }
}

// Fill matrix/vector with random values
void generateRandomValues(float* array, size_t size) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<float> dist(-1.0f, 1.0f);

    for (size_t i = 0; i < size; ++i) {
        array[i] = dist(gen);
    }
}

// Entrypoint
int main(int argc, char** argv) {
    // Inititalization
    MPI_Init(&argc, &argv);

    int procCount;
    int mpiRank;
    // Get number of processes
    MPI_Comm_size(MPI_COMM_WORLD, &procCount);
    MPI_Comm_rank(MPI_COMM_WORLD, &mpiRank);

    // Counters for time calculation
    double startTime;
    double endTime;
    double duration = 0;

    // Default matrix size
    int rows = 100, cols = 100;

    // Override default matrix size
    if (argc > 2) {
        rows = std::stoi(argv[1]);
        cols = std::stoi(argv[2]);
    }

    // Vector size must be equal to matrix columns count
    int vecSize = cols;
    int blocksOneDim = static_cast<int>(std::sqrt(procCount));
    int totalBlocks = blocksOneDim * blocksOneDim;
    int blockRows = rows / blocksOneDim;
    int blockCols = cols / blocksOneDim;

    // Create vars with corresponding types
    std::vector<float> matrix(rows * cols), vector(vecSize), localResult(rows), result;

    // If initial process
    if (mpiRank == 0) {
        matrix.resize(rows * cols);
        result.resize(rows); // Инициализируем result с нужным размером

        generateRandomValues(matrix.data(), rows * cols);
        generateRandomValues(vector.data(), vecSize);

#ifdef DEBUG
        // Print the initial matrix
        std::cout << "Initial Matrix: \n";
        for (size_t i = 0; i < rows; ++i) {
            for (size_t j = 0; j < cols; ++j) {
                std::cout << matrix[i * cols + j] << " ";
            }
            std::cout << "\n";
        }

        // Print the initial vector
        std::cout << "Initial Vector: \n";
        for (size_t i = 0; i < vecSize; ++i) {
            std::cout << vector[i] << " ";
        }
        std::cout << "\n";
#endif
    }

    // Ensure matrix and vector are properly initialized before broadcasting
    MPI_Bcast(vector.data(), vecSize, MPI_FLOAT, 0, MPI_COMM_WORLD);

    MPI_Bcast(matrix.data(), rows * cols, MPI_FLOAT, 0, MPI_COMM_WORLD);

    size_t startRow = (mpiRank / blocksOneDim) * blockRows;
    size_t endRow = startRow + blockRows;
    size_t startCol = (mpiRank % blocksOneDim) * blockCols;
    size_t endCol = startCol + blockCols;

    if (mpiRank < totalBlocks) {
        if (mpiRank == totalBlocks - 1) {
            endRow = rows;
            endCol = cols;
        } else if ((mpiRank + 1) % blocksOneDim == 0) {
            endCol = cols;
        } else if ((mpiRank + 1) > totalBlocks - blocksOneDim) {
            endRow = rows;
        }
    }

    // Perf test
    size_t iters = 100;
    for (size_t i = 0; i < iters; ++i) {
        startTime = MPI_Wtime();
        if (mpiRank < totalBlocks) {
            matrixVectorMultiplyByBlocks(matrix.data(), vector.data(), localResult.data(), cols, startRow, endRow, startCol, endCol);
        }
        endTime = MPI_Wtime();
        duration += endTime - startTime;
        // Clear vector
        std::fill(localResult.begin(), localResult.end(), 0);
    }
    duration = duration / static_cast<double>(iters);

    if (mpiRank < totalBlocks) {
        matrixVectorMultiplyByBlocks(matrix.data(), vector.data(), localResult.data(), cols, startRow, endRow, startCol, endCol);
    }

    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Reduce(localResult.data(), result.data(), rows, MPI_FLOAT, MPI_SUM, 0, MPI_COMM_WORLD);

    // If initial process
    if (mpiRank == 0) {
#ifdef DEBUG
        // Print the resulting matrix
        std::cout << "Resulting Matrix: \n";
        for (size_t i = 0; i < rows; ++i) {
            std::cout << result[i] << " ";
            if ((i + 1) % cols == 0) {  // Move to the next line after every 'cols' elements
                std::cout << "\n";
            }
        }
#endif
    }

    // Print time
    double maxTime = 0;
    MPI_Reduce(&duration, &maxTime, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
    if (mpiRank == 0) {
        std::cout << "Elapsed time: " << maxTime * 1000.0 << " ms\n";
    }

    MPI_Finalize();
    return 0;
}
