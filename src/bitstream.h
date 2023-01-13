#ifndef BITSTREAM_H
#define BITSTREAM_H

#include <stdio.h>

typedef struct bitstream bitstream;

bitstream *bitstream_create(FILE *fp);
void bitstream_destroy(bitstream *str);

int bitstream_get_bit(bitstream *str);
int bitstream_get_bits(bitstream *str, int n);

#endif
