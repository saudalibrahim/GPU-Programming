from numba import cuda,types
import numpy as np
import time
from numba import prange
def setup(array, num_obstacles):
    np.random.seed(42)
    indices = np.random.choice(array.size, num_obstacles, replace=True)
    # Use Numba's prange for parallelization
    for i in prange(num_obstacles):
        flat_index = indices[i]
        array.flat[flat_index] = 1
@cuda.jit
def findObstaclesCUDA(array, result):
 
    grid_row, grid_col = cuda.grid(2)
    stride_row, stride_col = cuda.gridsize(2)
 
    obstacle_count = 0
    for i in range(grid_row, array.shape[0], stride_row):
        for j in range(grid_col, array.shape[1], stride_col):
            if array[i, j] == 1:
                result[obstacle_count, 0] = i
                result[obstacle_count, 1] = j
                obstacle_count += 1

    # No need for synchronization, as each thread works on its own subset
if __name__ == "__main__":
    array_size = 50000
    num_obstacles = 100  # You might need to adjust this based on the array size
 
    array = np.zeros((array_size, array_size), dtype=np.int8)

    setup(array, num_obstacles)
    result_cuda = np.full((num_obstacles, 2), -1, dtype=np.int32)
    start_cuda = time.time()
    # Define thread block and grid dimensions
    threads_per_block = (16, 16)
    blocks_per_grid_x = (array_size + threads_per_block[0] - 1) // threads_per_block[0]
    blocks_per_grid_y = (array_size + threads_per_block[1] - 1) // threads_per_block[1]
    blocks_per_grid = (blocks_per_grid_x, blocks_per_grid_y)
    # Launch the CUDA kernel
    findObstaclesCUDA[blocks_per_grid, threads_per_block](array, result_cuda)
    end_cuda = time.time()
    duration_cuda = (end_cuda - start_cuda) * 1000
    print("CUDA Python Execution Time:", duration_cuda, "ms")