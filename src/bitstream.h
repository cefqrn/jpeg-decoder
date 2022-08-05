#ifndef BITSTREAM_H
#define BITSTREAM_H

#include <stdio.h>

typedef struct h_bit_stream bit_stream;

bit_stream *str_create_bit_stream(FILE *fp);
void str_free_bit_stream(bit_stream *str);
int str_get_bit(bit_stream *str);
int str_get_bits(bit_stream *str, int n);

#endif