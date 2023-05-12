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

    jpeg_info jpegInfo;
    scan_info scanInfo;
    if (jpeg_read_info(&jpegInfo, &scanInfo, fp)) {
        puts("Could not parse file.");
        goto FAIL_AFTER_READ_INFO;
    }

    pixel *im = malloc(image_size(jpegInfo.width, jpegInfo.height));
    if (im == NULL) {
        puts("Could not allocate memory for the image.");
        goto FAIL_AFTER_READ_INFO;
    }

    jpeg_read_image(im, &jpegInfo, &scanInfo, fp);

    image_ycbcr_to_rgb(im, jpegInfo.width, jpegInfo.height);
    image_print(im, jpegInfo.width, jpegInfo.height, 200, jpegInfo.height, 2);

    free(im);
    jpeg_free(&jpegInfo);

    return EXIT_SUCCESS;

FAIL_AFTER_READ_INFO:
    jpeg_free(&jpegInfo);
FAIL:
    return EXIT_FAILURE;
}
