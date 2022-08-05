#ifndef STREAM_H
#define STREAM_H

#include <stddef.h>
#include <stdint.h>

typedef struct h_bit_stream bit_stream;

bit_stream *str_create_stream(uint8_t *data, size_t length);
void str_free_stream(bit_stream *str);
int str_get_bit(bit_stream *str);
int str_get_bits(bit_stream *str, int n);

#endif