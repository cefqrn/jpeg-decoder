#include "bitstream.h"
#include "macros.h"

#include <stdint.h>
#include <stdio.h>

#define BYTE_WIDTH 8

static void load_char(bitstream *stream) {
    stream->_bitIndex = 0;

    // ignore byte stuffing
    if (stream->_c == 0xFF)
        getc(stream->_fp);

    stream->_c = getc(stream->_fp);

    CHECK_FAIL(ferror(stream->_fp), "Could not read file.");
    CHECK_FAIL(feof(stream->_fp),   "Reached end of file.");
}

bitstream bitstream_create(FILE *fp) {
    return (bitstream){
        ._fp       = fp,
        ._bitIndex = BYTE_WIDTH  // Make the bitstream_get_bit function load in a character when it's first called.
    };
}

// Returns the next bit of the file.
unsigned bitstream_get_bit(bitstream *stream) {
    // If all of the bits in the current char have been outputted, move to the next char.
    if (stream->_bitIndex == BYTE_WIDTH)
        load_char(stream);

    return (stream->_c >> (BYTE_WIDTH - ++stream->_bitIndex)) & 0x1;
}

// Returns the next count bits of the file in the stream as an integer.
// count must be 32 or less.
uint_fast32_t bitstream_get_bits(bitstream *stream, unsigned count) {
    CHECK_FAIL(count > 32, "Can't get more than 32 bits from a stream at a time.");

    uint_fast32_t value = 0;
    for (unsigned i=0; i < count; ++i)
        value = (value << 1) | bitstream_get_bit(stream);

    return value;
}
