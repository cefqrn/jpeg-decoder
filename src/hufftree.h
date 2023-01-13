#ifndef HUFFTREE_H
#define HUFFTREE_H

#include "bitstream.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef enum HuffTableClass {
    CLASS_DC = 0,
    CLASS_AC = 1
} HuffTableClass;

typedef struct huffnode {
    struct huffnode *left;
    struct huffnode *right;
    uint8_t symbol;
    bool hasLeft;
    bool hasRight;
    bool hasSymbol;
} huffnode;

size_t hufftree_parse(huffnode *buf, uint8_t *data);
void hufftree_destroy(huffnode *tree);

uint8_t hufftree_decode_next_symbol(huffnode *tree, bitstream *str);

#endif
