#include "macros.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <err.h>

#define CHAR_WIDTH 8

typedef struct h_stream {
    uint8_t *data;
    size_t length;
    size_t charIndex;
    size_t bitIndex;
} stream;

void str_print_stream(stream *str) {
    for (size_t i=0; i < str->length; i += 2) {
        for (size_t j=0; j < 2 && i + j < str->length; ++j) {
            printf("%02X", str->data[i + j]);
        }

        printf(" ");
    }
}

stream *str_create_stream(uint8_t *data, size_t length) {
    if (data == NULL)
        errx(EXIT_FAILURE, "Could not create stream with null data.");

    if (length == 0)
        errx(EXIT_FAILURE, "Could not create stream of length 0");

    stream *str = calloc(length, sizeof *str);
    CHECK_ALLOC(str, "stream");

    str->length = length;

    str->data = malloc(str->length * sizeof *str->data);
    CHECK_ALLOC(str->data, "stream data");

    memcpy(str->data, data, str->length);

    return str;
}

void str_free_stream(stream *str) {
    free(str->data);
    free(str);
}

int str_get_bit(stream *str) {
    // returns the next bit in st

    // if all of the bits in the current char have been outputted, move to the next char
    if (str->bitIndex == CHAR_WIDTH) {
        str->bitIndex = 0;
        ++str->charIndex;
    }

    // check if stream is dry
    if (str->charIndex == str->length) {
        return -1;
    }

    return (str->data[str->charIndex] >> (CHAR_WIDTH - ++str->bitIndex)) & 0x1;
}

int str_get_bits(stream *str, int n) {
    int value = 0;
    for (int i=0; i < n; ++i) {
        value = (value << 1) | str_get_bit(str);
    }

    return value;
}