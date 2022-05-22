#include "image.h"
#include "jpeg.h"
#include <stdio.h>

int main(int argc, char *argv[]) {
    if (argc <= 1) {
        puts("No path given.");
        return -1;
    }

    char *path = argv[1];

    image *im = jpg_fparse(path);
    img_print_image(im);
    img_free_image(im);
}