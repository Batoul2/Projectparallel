#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

// Function to flip a chunk of the image horizontally
void flip_horizontal_chunk(int *img_chunk, int rows, int cols) {
    #pragma omp parallel for collapse(2) num_threads(4)
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols / 2; j++) {
            int temp = img_chunk[i * cols + j];
            img_chunk[i * cols + j] = img_chunk[i * cols + (cols - 1 - j)];
            img_chunk[i * cols + (cols - 1 - j)] = temp;
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
        for (int j = 0; j < cols / 2; j++) {
            int temp = image_data[i * cols + j];
            image_data[i * cols + j] = image_data[i * cols + (cols - 1 - j)];
            image_data[i * cols + (cols - 1 - j)] = temp;
        }
    }
}

int main() {
    int rows, cols;
    int *image_data = NULL;

    // Load your own image using the load_image function
    // Adjust the parameters based on your image dimensions
    rows = 1000;  // Adjust based on your image dimensions
    cols = 1000;  // Adjust based on your image dimensions
    image_data = (int *)malloc(rows * cols * sizeof(int));
    load_image(image_data, rows, cols, "me.jpg");  // Provide the correct image path

    // Sequential Version
    double start_time_seq = omp_get_wtime();
    flip_image_sequential(image_data, rows, cols);
    double end_time_seq = omp_get_wtime();
    double sequential_time = end_time_seq - start_time_seq;

    // Parallel Version
    double start_time_parallel = omp_get_wtime();
    flip_horizontal_chunk(image_data, rows, cols);
    double end_time_parallel = omp_get_wtime();
    double parallel_time = end_time_parallel - start_time_parallel;

    // Calculate speedup and efficiency
    double speedup = sequential_time / parallel_time;
    double efficiency = speedup / 4;  // Assuming you want to use 4 threads

    // Output results
    printf("Using 4 threads\n");
    printf("Sequential Time: %f seconds\n", sequential_time);
    printf("Parallel Time: %f seconds\n", parallel_time);
    printf("Speedup: %f\n", speedup);
    printf("Efficiency: %f\n", efficiency);

    free(image_data);

    return 0;
}
