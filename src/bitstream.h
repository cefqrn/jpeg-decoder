#ifndef BITSTREAM_H
#define BITSTREAM_H

#include <stdint.h>
#include <stdio.h>

typedef struct {
    FILE          *_fp;
    unsigned       _bitIndex;
    unsigned char  _c;
} bitstream;

bitstream bitstream_create(FILE *fp);

unsigned      bitstream_get_bit(bitstream *str);
uint_fast32_t bitstream_get_bits(bitstream *str, unsigned count);

#endif
