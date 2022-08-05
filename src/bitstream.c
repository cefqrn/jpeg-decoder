#include "bitstream.h"
#include "macros.h"
#include "stdio.h"
#include <stdlib.h>
#include <string.h>

#define CHAR_WIDTH 8

typedef struct h_bit_stream {
    FILE *fp;
    char c;
    size_t bitIndex;
} bit_stream;

bit_stream *str_create_bit_stream(FILE *fp) {
    bit_stream *str = calloc(1, sizeof *str);
    CHECK_ALLOC(str, "stream");

    str->fp = fp;
    str->bitIndex = 0;

    return str;
}

// returns the next bit in stream
int str_get_bit(bit_stream *stream) {
    // if all of the bits in the current char have been outputted, move to the next char
    if (stream->bitIndex == CHAR_WIDTH) {
        stream->bitIndex = 0;
        
        char c = getc(stream->fp);
        
        // ignore byte stuffing
        stream->c = stream->c == (char)0xFF ? getc(stream->fp) : c;

        CHECK_FAIL(feof(stream->fp), "Tried to get bit from dry stream.");
    }

    return (stream->c >> (CHAR_WIDTH - ++stream->bitIndex)) & 0x1;
}

// returns the next n bits in stream and converts returns the bit array as an integer
int str_get_bits(bit_stream *stream, int n) {
    int value = 0;
    for (int i=0; i < n; ++i) {
        value = (value << 1) | str_get_bit(stream);
    }

    return value;
}

void str_free_bit_stream(bit_stream *stream) {
    free(stream);
}