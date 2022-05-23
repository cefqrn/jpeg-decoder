#include "macros.h"
#include "image.h"
#include "jpeg.h"
#include <stdio.h>

int main(int argc, char *argv[]) {
    CHECK_FAIL(argc <= 1, "No path given.");

    char *path = argv[1];

    image *im = jpg_fparse(path);

    img_yuv_to_rgb(im);
    img_print_image(im, 2);

    img_free_image(im);
}