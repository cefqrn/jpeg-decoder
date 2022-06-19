#include "quanttable.h"
#include "macros.h"
#include <stdlib.h>

quant_table *qnt_parse_quant_table(uint8_t *data, size_t *offset) {
    quant_table *table = malloc(sizeof *table);
    CHECK_ALLOC(table, "quantization table");

    uint8_t info = data[(*offset)++];

    table->precision = (info & 0x10) >> 4;
    table->number = info & 0x1;

    size_t dataSize =  table->precision == 0 ? sizeof(uint8_t) : sizeof(uint16_t);

    for (size_t i=0; i < QNT_VALUE_COUNT; ++i) {
        table->values[i] = (data + (*offset)++)[i * (dataSize - 1)];
    }

    return table;
}

void qnt_free_quant_table(quant_table *table) {
    free(table);
}