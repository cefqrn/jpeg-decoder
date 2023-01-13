#include "macros.h"
#include "image.h"
#include "jpeg.h"

#include <stdio.h>

int main(int argc, char *argv[]) {
    CHECK_FAIL(argc <= 1, "No path given.");

    char *path = argv[1];

    image *im = jpeg_fparse(path);

    image_yuv_to_rgb(im);
    image_print(im, 2, 200, im->height);

    image_destroy(im);
}
