#include "bitstream.h"
#include "macros.h"

#include <stdlib.h>
#include <stdio.h>

#define CHAR_WIDTH 8

struct bitstream {
    FILE *fp;
    char c;
    size_t bitIndex;
};

static void get_next_char(bitstream *stream) {
    stream->bitIndex = 0;
    
    // ignore byte stuffing
    if (stream->c == (char)0xFF) {
        getc(stream->fp);
    }

    stream->c = getc(stream->fp);
    CHECK_FAIL(feof(stream->fp), "Tried to get bit from empty stream.");
}

bitstream *bitstream_create(FILE *fp) {
    bitstream *stream = calloc(1, sizeof *stream);
    CHECK_ALLOC(stream, "bitstream");

    stream->fp = fp;
    stream->bitIndex = 0;

    // load in first char of the stream
    get_next_char(stream);

    return stream;
}

// returns the next bit in stream
int bitstream_get_bit(bitstream *stream) {
    // if all of the bits in the current char have been outputted, move to the next char
    if (stream->bitIndex == CHAR_WIDTH) get_next_char(stream);

    return (stream->c >> (CHAR_WIDTH - ++stream->bitIndex)) & 0x1;
}

// returns the next n bits in stream and returns the bit array as an integer
int bitstream_get_bits(bitstream *stream, int n) {
    int value = 0;
    for (int i=0; i < n; ++i) {
        value = (value << 1) | bitstream_get_bit(stream);
    }

    return value;
}

void bitstream_destroy(bitstream *stream) {
    free(stream);
}
