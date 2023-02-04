#include "image.h"
#include "jpeg.h"

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
    if (argc <= 1) {
        puts("No path given.");
        return EXIT_FAILURE;
    }

    FILE *fp = fopen(argv[1], "r");
    if (fp == NULL) {
        printf("Could not open file: %s\n", strerror(errno));
        return EXIT_FAILURE;
    }

    jpeg_info info;
    jpeg_read_info(&info, fp);

    pixel *im = malloc(image_size(info.width, info.height));
    if (im == NULL) {
        puts("Could not allocate memory for the image.");
        return EXIT_FAILURE;
    }

    jpeg_read_image(im, &info, fp);

    image_yuv_to_rgb(im, info.width, info.height);
    image_print(im, info.width, info.height, 200, info.height, 2);

    free(im);
    jpeg_free(&info);

    return EXIT_SUCCESS;
}
