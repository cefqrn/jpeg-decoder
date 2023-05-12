#ifndef DECODING_H
#define DECODING_H

#include "bitstream.h"
#include "image.h"
#include "jpeg.h"

#include <stddef.h>

int decode_data_unit(pixel *im, const jpeg_info *info, bitstream *str, component_data componentData, int *dcCoeff, unsigned globalX, unsigned globalY, unsigned hSamplingFactor, unsigned vSamplingFactor);

#endif
