from google.colab import files

# Upload the image
uploaded = files.upload()

# Get the file name
image_filename = list(uploaded.keys())[0]

%%writefile cuda_code.cu

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cuda_runtime.h>

_global_ void flip_horizontal_chunk(int *img_chunk, int rows, int cols) {
    int i = blockIdx.x * blockDim.x + threadIdx.x;
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

void load_image(int *image_data, int rows, int cols, const char *file_path) {
    FILE *file = fopen(file_path, "rb");
    if (!file) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    fread(image_data, sizeof(int), rows * cols, file);

    fclose(file);
}

int main() {
    int rows, cols;
    int *image_data = NULL;

    const char *uploaded_file_path = "me.jpg";  // Set the uploaded file path
    rows = 1000;  // Adjust based on your image dimensions
    cols = 1000;  // Adjust based on your image dimensions
    size_t img_size = rows * cols * sizeof(int);
    image_data = (int *)malloc(img_size);
    load_image(image_data, rows, cols, uploaded_file_path);  // Use the uploaded image path

    // Sequential Part
    cudaEvent_t startSeq, stopSeq;
    cudaEventCreate(&startSeq);
    cudaEventCreate(&stopSeq);
    cudaEventRecord(startSeq);

    // ... Your sequential code here ...

    cudaEventRecord(stopSeq);
    cudaEventSynchronize(stopSeq);
    float seqElapsedTime;
    cudaEventElapsedTime(&seqElapsedTime, startSeq, stopSeq);

    printf("Sequential Time: %f ms\n", seqElapsedTime);

    // Parallel Part
    int *d_image_data;
    cudaMalloc((void **)&d_image_data, img_size);

    cudaMemcpy(d_image_data, image_data, img_size, cudaMemcpyHostToDevice);

    cudaEvent_t startPar, stopPar;
    cudaEventCreate(&startPar);
    cudaEventCreate(&stopPar);
    cudaEventRecord(startPar);

    int block_size = 256;
    int grid_size = (rows + block_size - 1) / block_size;

    flip_horizontal_chunk<<<grid_size, block_size>>>(d_image_data, rows, cols);

    cudaEventRecord(stopPar);
    cudaEventSynchronize(stopPar);
    float parElapsedTime;
    cudaEventElapsedTime(&parElapsedTime, startPar, stopPar);

    printf("Parallel Time: %f ms\n", parElapsedTime);

    cudaMemcpy(image_data, d_image_data, img_size, cudaMemcpyDeviceToHost);

    cudaFree(d_image_data);

    free(image_data);

    return 0;
}
