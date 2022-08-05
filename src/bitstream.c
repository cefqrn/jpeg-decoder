#include "bitstream.h"
#include "macros.h"
#include <stdlib.h>
#include <string.h>

#define CHAR_WIDTH 8

typedef struct h_bit_stream {
    uint8_t *data;
    size_t length;
    size_t charIndex;
    size_t bitIndex;
} bit_stream;

bit_stream *str_create_stream(uint8_t *data, size_t length) {
    CHECK_FAIL(data == NULL, "Could not create stream with null data.");
    CHECK_FAIL(length == 0, "Could not create stream of length 0");

    bit_stream *str = calloc(length, sizeof *str);
    CHECK_ALLOC(str, "stream");

    str->length = length;

    str->data = malloc(str->length * sizeof *str->data);
    CHECK_ALLOC(str->data, "stream data");

    memcpy(str->data, data, str->length);

    return str;
}

// returns the next bit in str
int str_get_bit(bit_stream *str) {
    // if all of the bits in the current char have been outputted, move to the next char
    if (str->bitIndex == CHAR_WIDTH) {
        str->bitIndex = 0;
        ++str->charIndex;
    }

    // check if stream is dry
    CHECK_FAIL(str->charIndex == str->length, "Tried to get bit from dry stream.");

    return (str->data[str->charIndex] >> (CHAR_WIDTH - ++str->bitIndex)) & 0x1;
}

int str_get_bits(bit_stream *str, int n) {
    int value = 0;
    for (int i=0; i < n; ++i) {
        value = (value << 1) | str_get_bit(str);
    }

    return value;
}

void str_free_stream(bit_stream *str) {
    free(str->data);
    free(str);
}