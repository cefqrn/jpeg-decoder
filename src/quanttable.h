#ifndef QUANTTABLE_H
#define QUANTTABLE_H

#include <stddef.h>
#include <stdint.h>

typedef struct quant_table quant_table;

quant_table *qnt_parse_quant_table(uint8_t *data, size_t *offset);
void qnt_free_quant_table(quant_table *table);
void qnt_print_quant_table(quant_table *table);
uint8_t qnt_get_quant_table_number(quant_table *table);
uint16_t qnt_get_quant_table_value(quant_table *table, size_t index);

#endif