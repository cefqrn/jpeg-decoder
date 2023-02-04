#ifndef DECODING_H
#define DECODING_H

#include "bitstream.h"
#include "image.h"
#include "jpeg.h"

#include <stddef.h>

int decode_MCU(pixel *im, const jpeg_info *info, bitstream *str, component_data componentData, int prevDcCoeff, size_t McuX, size_t McuY, size_t hSamplingFactor, size_t vSamplingFactor);

#endif
