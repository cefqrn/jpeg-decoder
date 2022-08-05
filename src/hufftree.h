#ifndef HUFFTREE_H
#define HUFFTREE_H

#include "stream.h"
#include <stddef.h>
#include <stdint.h>

typedef enum HuffTableClass {
    CLASS_DC = 0,
    CLASS_AC = 1
} HuffTableClass;

typedef struct huff_node huff_node;

typedef struct huff_tree {
    huff_node *root;
    HuffTableClass class;
    uint8_t id;
} huff_tree;

huff_tree *huf_parse_huff_tree(uint8_t *data, size_t *offset);
void huf_free_huff_tree(huff_tree *tree);
uint8_t huf_decode_next_symbol(huff_tree *tree, bit_stream *str);

#endif