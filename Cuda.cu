#include <iostream>
#include <cuda_runtime.h>
#include <chrono>
#include <vector>
#include <random>
 
using namespace std;
using namespace chrono;
 
void setup(int* array, int arraySize, int numObstacles) {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<int> dis(0, arraySize - 1);
 
    for (int i = 0; i < numObstacles; ++i) {
        int row = dis(gen);
        int col = dis(gen);
        array[row * arraySize + col] = 1;
    }
}
 
__global__ void findObstaclesCUDA(int* array, int arraySize, int* result) {
    int obstacleCount = 0;
 
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    int stride = gridDim.x * blockDim.x;
 
    for (int i = idx; i < arraySize * arraySize; i += stride) {
        int row = i / arraySize;
        int col = i % arraySize;
 
        if (array[row * arraySize + col] == 1) {
            int index = atomicAdd(&obstacleCount, 1);
            result[index * 2] = row;
            result[index * 2 + 1] = col;
        }
    }
}
 
int main() {
    int arraySize = 256;
    int numObstacles = 100;
 
    int* array = new int[arraySize * arraySize];
    cudaMallocManaged(&array, arraySize * arraySize * sizeof(int));
 
    auto startCUDA = high_resolution_clock::now();
 
    setup(array, arraySize, numObstacles);
 
    int* resultCUDA;
    cudaMallocManaged(&resultCUDA, numObstacles * 2 * sizeof(int));
 
    int threadsPerBlock = 256;
    int blocksPerGrid = (arraySize * arraySize + threadsPerBlock - 1) / threadsPerBlock;
    findObstaclesCUDA<<<blocksPerGrid, threadsPerBlock>>>(array, arraySize, resultCUDA);
    cudaDeviceSynchronize();
 
    cudaError_t cudaError;
    cudaError = cudaGetLastError();
    if (cudaError != cudaSuccess) {
        cout << "CUDA error: " << cudaGetErrorString(cudaError) << endl;
    }
 
    auto stopCUDA = high_resolution_clock::now();
    auto durationCUDA = duration_cast<milliseconds>(stopCUDA - startCUDA);
 
    cout << "CUDA C/C++ Execution Time: " << durationCUDA.count() << " ms\n";
 
    delete[] array;
    cudaFree(resultCUDA);
 
    return 0;
}
