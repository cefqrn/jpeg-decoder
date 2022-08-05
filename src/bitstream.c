#include "bitstream.h"
#include "macros.h"
#include "stdio.h"
#include <stdlib.h>

#define CHAR_WIDTH 8

typedef struct h_bit_stream {
    FILE *fp;
    char c;
    size_t bitIndex;
} bit_stream;

static void get_next_char(bit_stream *stream) {
    stream->bitIndex = 0;
    
    // ignore byte stuffing
    if (stream->c == (char)0xFF) {
        getc(stream->fp);
    }

    stream->c = getc(stream->fp);
    CHECK_FAIL(feof(stream->fp), "Tried to get bit from dry stream.");
}

bit_stream *str_create_bit_stream(FILE *fp) {
    bit_stream *stream = calloc(1, sizeof *stream);
    CHECK_ALLOC(stream, "stream");

    stream->fp = fp;
    stream->bitIndex = 0;

    // load in first char of the stream
    get_next_char(stream);

    return stream;
}

// returns the next bit in stream
int str_get_bit(bit_stream *stream) {
    // if all of the bits in the current char have been outputted, move to the next char
    if (stream->bitIndex == CHAR_WIDTH) get_next_char(stream);

    return (stream->c >> (CHAR_WIDTH - ++stream->bitIndex)) & 0x1;
}

// returns the next n bits in stream and returns the bit array as an integer
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