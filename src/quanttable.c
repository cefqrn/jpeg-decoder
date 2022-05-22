#include "macros.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <err.h>

#define VALUE_COUNT 64

typedef struct quant_table {
    uint16_t values[VALUE_COUNT];
    uint8_t precision:4;
    uint8_t number:4;
} quant_table;

void qnt_print_quant_table(quant_table *table) {
    for (size_t i=0; i < 8; ++i) {
        for (size_t j=0; j < 8; ++j) {
            printf("%02X ", table->values[i*8 + j]);
        }

        printf("\n");
    }
}

quant_table *qnt_parse_quant_table(uint8_t *data, size_t *offset) {
    quant_table *table = malloc(sizeof *table);
    CHECK_ALLOC(table, "quantization table");

    uint8_t info = data[(*offset)++];

    table->precision = (info & 0x10) >> 4;
    table->number = info & 0x1;

    size_t dataSize;
    if (table->precision == 0) {
        dataSize = sizeof(uint8_t);
    } else {
        dataSize = sizeof(uint16_t);
    }

    for (size_t i=0; i < VALUE_COUNT; ++i) {
        table->values[i] = (data + (*offset)++)[i * (dataSize - 1)];
    }

    return table;
}

void qnt_free_quant_table(quant_table *table) {
    free(table);
}

uint8_t qnt_get_quant_table_number(quant_table *table) {
    return table->number;
}

uint16_t qnt_get_quant_table_value(quant_table *table, size_t index) {
    return table->values[index];
}