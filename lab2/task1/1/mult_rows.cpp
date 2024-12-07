#include <iostream>
#include <mpi.h>
#include <string>
#include <vector>
#include <random>
#include <algorithm>

// Calculate matrix multiplication
void matrixVectorMultiplyByRows(const float* matrix, const float* vector, float* result, size_t rows, size_t cols) {
    for (size_t i = 0; i < rows; ++i) {
        result[i] = 0;
        for (size_t j = 0; j < cols; ++j) {
            result[i] = result[i] + (matrix[i * cols + j] * vector[j]);
        }
    }
}

// Fill matrix/vector with random values
void generateRandomValues(float* array, size_t size) {
    static std::random_device rd;  // Только один объект random_device
    static std::mt19937 gen(rd()); // Генератор случайных чисел
    static std::uniform_real_distribution<float> dist(-1.0f, 1.0f); // Распределение

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
    // Share rows between procCount processes
    int interimRows = rows / procCount;
    // Total matrix elements count
    int interimElements = interimRows * cols;

    // Create vars with corresponding types
    std::vector<float> matrix, vector(vecSize), localMatrix(interimElements), localResult(interimRows), result;

    // If initial process
    if (mpiRank == 0) {
        matrix.resize(rows * cols);
        result.resize(rows);

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

    MPI_Scatter(matrix.data(), interimElements, MPI_FLOAT, localMatrix.data(), interimElements, MPI_FLOAT, 0, MPI_COMM_WORLD);
    MPI_Bcast(vector.data(), vecSize, MPI_FLOAT, 0, MPI_COMM_WORLD);

    // Perf test
    size_t iters = 100;
    for (size_t i = 0; i < iters; ++i) {
        startTime = MPI_Wtime();
        matrixVectorMultiplyByRows(localMatrix.data(), vector.data(), localResult.data(), interimRows, cols);
        endTime = MPI_Wtime();
        duration = duration + (endTime - startTime);
        // Clear vector
        std::fill(localResult.begin(), localResult.end(), 0);
    }
    duration = duration / static_cast<double>(iters);

    matrixVectorMultiplyByRows(localMatrix.data(), vector.data(), localResult.data(), interimRows, cols);

    if (mpiRank == 0) {
        std::cout << "Elapsed time: " << duration * 1000.0 << " ms\n";
    }

    // Results aggregation
    MPI_Gather(localResult.data(), interimRows, MPI_FLOAT, result.data(), interimRows, MPI_FLOAT, 0, MPI_COMM_WORLD);

    // If initial process
    if (mpiRank == 0) {
        size_t remainingRows = rows % procCount;
        if (remainingRows >= 1) {
            size_t offset = rows - remainingRows;
            startTime = MPI_Wtime();
            matrixVectorMultiplyByRows(matrix.data() + cols * offset, vector.data(), result.data() + offset, remainingRows, cols);
            endTime = MPI_Wtime();
            duration = duration + (endTime - startTime);
        }

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

    MPI_Finalize();
    return 0;
}
