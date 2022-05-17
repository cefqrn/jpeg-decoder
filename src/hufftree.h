#ifndef HUFFTREE_H
#define HUFFTREE_H

#include <stdint.h>
#include <stddef.h>

typedef struct huff_tree huff_tree;

huff_tree *huf_parse_huff_tree(uint8_t *data, size_t *offset);
void huf_free_huff_tree(huff_tree *tree);
void huf_print_huff_tree(huff_tree *tree);
uint8_t huf_get_huff_tree_class(huff_tree *tree);
uint8_t huf_get_huff_tree_id(huff_tree *tree);

#endif