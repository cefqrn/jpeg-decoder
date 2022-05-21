#ifndef STREAM_H
#define STREAM_H

#include <stddef.h>
#include <stdint.h>

typedef struct h_stream stream;

void str_print_stream(stream *str);
stream *str_create_stream(uint8_t *data, size_t length);
void str_free_stream(stream *str);
int str_get_bit(stream *str);
int str_get_bits(stream *str, int n);

#endif