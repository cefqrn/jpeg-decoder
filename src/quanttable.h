#ifndef QUANTTABLE_H
#define QUANTTABLE_H

#include <stddef.h>
#include <stdint.h>

#define QNT_VALUE_COUNT 64

typedef struct quant_table {
    uint16_t values[QNT_VALUE_COUNT];
    uint8_t precision:4;
    uint8_t number:4;
} quant_table;

quant_table *qnt_parse_quant_table(uint8_t *data, size_t *offset);
void qnt_free_quant_table(quant_table *table);

#endif