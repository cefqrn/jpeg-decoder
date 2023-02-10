#include "image.h"
#include "jpeg.h"

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
    if (argc <= 1) {
        puts("No path given.");
        goto FAIL;
    }

    FILE *fp = fopen(argv[1], "r");
    if (fp == NULL) {
        printf("Could not open file: %s\n", strerror(errno));
        goto FAIL;
    }

    jpeg_info info;
    if (jpeg_read_info(&info, fp)) {
        puts("Could not parse file.");
        goto FAIL_READ_INFO;
    }

    pixel *im = malloc(image_size(info.width, info.height));
    if (im == NULL) {
        puts("Could not allocate memory for the image.");
        goto FAIL_READ_INFO;
    }

    if (jpeg_read_image(im, &info, fp)) {
        puts("Could not read image.");
        goto FAIL_ALLOCATED_IMAGE;
    };

    image_yuv_to_rgb(im, info.width, info.height);
    image_print(im, info.width, info.height, 200, info.height, 2);

    free(im);
    jpeg_free(&info);

    return EXIT_SUCCESS;

FAIL_ALLOCATED_IMAGE:
    free(im);
FAIL_READ_INFO:
    jpeg_free(&info);
FAIL:
    return EXIT_FAILURE;
}
