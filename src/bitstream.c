#include "bitstream.h"
#include "macros.h"

#include <stdint.h>
#include <stdio.h>

static void load_char(bitstream *stream) {
    stream->_bitsLeft = 8;

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
        ._bitsLeft = 0
    };
}

// Returns the next bit of the file.
unsigned bitstream_get_bit(bitstream *stream) {
    if (stream->_bitsLeft == 0)
        load_char(stream);

    return (stream->_c >> --(stream->_bitsLeft)) & 0x1;
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
