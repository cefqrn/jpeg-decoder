#ifndef DECODING_H
#define DECODING_H

#include "bitstream.h"
#include "image.h"
#include "jpeg.h"

#include <stddef.h>

void decode_data_unit(pixel *im, const jpeg_info *info, struct scan_component_info componentInfo, bitstream *str, int *dcCoeff, unsigned globalX, unsigned globalY, unsigned HStretchFactor, unsigned VStretchFactor);

#endif
