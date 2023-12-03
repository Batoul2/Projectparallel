#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define MASTER_RANK 0

// Function to flip a chunk of the image horizontally
void flip_horizontal_chunk(int *img_chunk, int rows, int cols) {
    for (int i = 0; i < rows; i++) {
        int row_start = i * cols;
        int row_end = (i + 1) * cols - 1;
        while (row_start < row_end) {
            int temp = img_chunk[row_start];
            img_chunk[row_start] = img_chunk[row_end];
            img_chunk[row_end] = temp;
            row_start++;
            row_end--;
        }
    }
}

// Function to load image data from a binary file
void load_image(int *image_data, int rows, int cols, const char *file_path) {
    FILE *file = fopen(file_path, "rb");
    if (!file) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    fread(image_data, sizeof(int), rows * cols, file);

    fclose(file);
}

// Function to flip the entire image sequentially
void flip_image_sequential(int *image_data, int rows, int cols) {
    for (int i = 0; i < rows; i++) {
        flip_horizontal_chunk(&image_data[i * cols], 1, cols);
    }
}

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);

    int world_size, world_rank;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    int rows, cols;
    int *image_data = NULL;

    if (world_rank == MASTER_RANK) {
        // Load your own image using the load_image function
        // Adjust the parameters based on your image dimensions
        rows = 1000;  // Adjust based on your image dimensions
        cols = 1000;  // Adjust based on your image dimensions
        image_data = (int *)malloc(rows * cols * sizeof(int));
        load_image(image_data, rows, cols, "me.jpg");  // Provide the correct image path
    }

    MPI_Bcast(&rows, 1, MPI_INT, MASTER_RANK, MPI_COMM_WORLD);
    MPI_Bcast(&cols, 1, MPI_INT, MASTER_RANK, MPI_COMM_WORLD);

    int rows_per_process = rows / world_size;
    int *image_chunk = (int *)malloc(rows_per_process * cols * sizeof(int));

    MPI_Scatter(image_data, rows_per_process * cols, MPI_INT, image_chunk, rows_per_process * cols, MPI_INT, MASTER_RANK, MPI_COMM_WORLD);

    MPI_Barrier(MPI_COMM_WORLD);
    double start_time = MPI_Wtime();

    // Perform the parallel image flipping (horizontal flip in this case)
    flip_horizontal_chunk(image_chunk, rows_per_process, cols);

    MPI_Barrier(MPI_COMM_WORLD);
    double end_time = MPI_Wtime();

    MPI_Gather(image_chunk, rows_per_process * cols, MPI_INT, image_data, rows_per_process * cols, MPI_INT, MASTER_RANK, MPI_COMM_WORLD);

    if (world_rank == MASTER_RANK) {
        // Output the flipped image on the master process
        // printf("Original Image:\n");  // Uncomment if you want to print the original image
        // for (int i = 0; i < rows * cols; i++) {
        //     printf("%d ", image_data[i]);
        //     if ((i + 1) % cols == 0) {
        //         printf("\n");
        //     }
        // }

        // Calculate parallel time
        double parallel_time = end_time - start_time;
        printf("\nParallel Time: %f seconds\n", parallel_time);

        // Sequential timing on the master process
        double start_time_seq = MPI_Wtime();
        flip_image_sequential(image_data, rows, cols);
        double end_time_seq = MPI_Wtime();
        // Calculate sequential time
        double sequential_time = end_time_seq - start_time_seq;
        printf("Sequential Time: %f seconds\n", sequential_time);

        // Calculate speedup and efficiency
        double speedup = sequential_time / parallel_time;
        double efficiency = speedup / world_size;
        printf("Speedup: %f\n", speedup);
        printf("Efficiency: %f\n", efficiency);

        // Uncomment if you want to free the image data
        // free(image_data);
    }

    free(image_chunk);
    MPI_Finalize();

    return 0;
}
