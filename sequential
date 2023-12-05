#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void flipHorizontally(unsigned char* image, int width, int height) {
    int bytesPerPixel = 3; // Assuming RGB image with 3 bytes per pixel

    for (int row = 0; row < height; row++) {
        for (int col = 0; col < width / 2; col++) {
            for (int channel = 0; channel < bytesPerPixel; channel++) {
                int leftIndex = (row * width + col) * bytesPerPixel + channel;
                int rightIndex = (row * width + (width - 1 - col)) * bytesPerPixel + channel;

                // Swap pixels horizontally
                unsigned char temp = image[leftIndex];
                image[leftIndex] = image[rightIndex];
                image[rightIndex] = temp;
            }
        }
    }
}

void flipVertically(unsigned char* image, int width, int height) {
    int bytesPerPixel = 3; // Assuming RGB image with 3 bytes per pixel

    for (int row = 0; row < height / 2; row++) {
        for (int col = 0; col < width; col++) {
            for (int channel = 0; channel < bytesPerPixel; channel++) {
                int topIndex = (row * width + col) * bytesPerPixel + channel;
                int bottomIndex = ((height - 1 - row) * width + col) * bytesPerPixel + channel;

                // Swap pixels vertically
                unsigned char temp = image[topIndex];
                image[topIndex] = image[bottomIndex];
                image[bottomIndex] = temp;
            }
        }
    }
}

int main() {
    // Example image size (replace with your actual image dimensions)
    int width = 5000;
    int height = 2000;

    // Allocate memory for the image (assuming RGB image with 3 bytes per pixel)
    unsigned char* image = (unsigned char*)malloc(width * height * 3);

    // Initialize the image data (replace with your actual image data)

    // Record start time
    clock_t start = clock();

    // Flip horizontally
    flipHorizontally(image, width, height);

    // Record end time
    clock_t end = clock();

    // Calculate elapsed time in seconds
    double elapsedTime = ((double)(end - start)) / CLOCKS_PER_SEC;

    printf("Time taken to flip horizontally: %f seconds\n", elapsedTime);

    // Record start time
    start = clock();

    // Flip vertically
    flipVertically(image, width, height);

    // Record end time
    end = clock();

    // Calculate elapsed time in seconds
    elapsedTime = ((double)(end - start)) / CLOCKS_PER_SEC;

    printf("Time taken to flip vertically: %f seconds\n", elapsedTime);

    // Free allocated memory
    free(image);

    return 0;
}
