#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#define VALUE_OFFSET 1
#define VALUE_COUNT 64

typedef struct quant_table {
    uint16_t values[VALUE_COUNT];
    uint8_t precision:4;
    uint8_t number:4;
} quant_table;

quant_table *qnt_parse_quant_table(uint8_t *data) {
    quant_table *table = malloc(sizeof *table);

    table->precision = data[0] & 0x10 >> 4;
    table->number = data[0] & 0x1;

    size_t dataSize;
    if (table->precision == 0) {
        dataSize = sizeof(uint8_t);
    } else {
        dataSize = sizeof(uint16_t);
    }

    for (size_t i=0; i < VALUE_COUNT * dataSize; i += dataSize) {
        table->values[i] = 0;
        for (size_t j=0; j < dataSize; ++j) {
            table->values[i] += (data + VALUE_OFFSET)[i + j] << (dataSize - j + 1);
        }
    }

    return table;
}

void qnt_free_quant_table(quant_table *table) {
    free(table);
}

void qnt_print_quant_table(quant_table *table) {
    for (size_t i=0; i < 8; ++i) {
        for (size_t j=0; j < 8; ++j) {
            printf("%02X ", table->values[i*8 + j]);
        }

        printf("\n");
    }
}

uint8_t qnt_get_quant_table_number(quant_table *table) {
    return table->number;
}