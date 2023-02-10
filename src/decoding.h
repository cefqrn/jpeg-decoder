#ifndef DECODING_H
#define DECODING_H

#include "bitstream.h"
#include "image.h"
#include "jpeg.h"

#include <stddef.h>

int decode_MCU(pixel *im, const jpeg_info *info, bitstream *str, component_data componentData, int *dcCoeff, unsigned McuX, unsigned McuY, unsigned hSamplingFactor, unsigned vSamplingFactor);

#endif
