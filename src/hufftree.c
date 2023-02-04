#include "bitstream.h"
#include "hufftree.h"
#include "macros.h"

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define CODE_LENGTH_COUNT 16

static huffnode *create_huff_node() {
    huffnode *node = calloc(1, sizeof(huffnode));
    CHECK_ALLOC(node, "huffnode");

    return node;
}

static void add_symbol_to_huff_node(huffnode *root, unsigned short code, unsigned codeLength, uint8_t symbol) {
    huffnode *curr = root;

    for (unsigned i=codeLength; i > 0; --i) {
        if ((code >> (i - 1)) & 0x1) {
            if (!curr->hasRight) {
                curr->right = create_huff_node();
                curr->hasRight = true;
            }

            curr = curr->right;
        } else {
            if (!curr->hasLeft) {
                curr->left = create_huff_node();
                curr->hasLeft = true;
            }

            curr = curr->left;
        }
    }

    curr->symbol = symbol;
    curr->hasSymbol = true;
}

size_t hufftree_parse(huffnode *buf, const unsigned char *data) {
    memset(buf, 0, sizeof *buf);

    size_t offset = CODE_LENGTH_COUNT;
    int codeValue = 0;
    for (size_t codeLength = 1; codeLength <= CODE_LENGTH_COUNT; ++codeLength) {
        size_t codeCount = data[codeLength-1];

        for (size_t i = 0; i < codeCount; ++i) {
            unsigned short code  = codeValue++;
            unsigned char symbol = data[offset++];

            add_symbol_to_huff_node(buf, code, codeLength, symbol);
        }

        codeValue <<= 1;
    }

    return offset;
}

uint8_t hufftree_decode_next_symbol(const huffnode *tree, bitstream *str) {
    const huffnode *curr = tree;

    do {
        if (bitstream_get_bit(str)) {
            CHECK_FAIL(!curr->hasRight, "Could not find next symbol in huffnode.");
            curr = curr->right;
        } else {
            CHECK_FAIL(!curr->hasLeft, "Could not find next symbol in huffnode.");
            curr = curr->left;
        }
    } while (!curr->hasSymbol);

    return curr->symbol;
}

static void free_node(huffnode *node) {
    if (node->hasLeft) free_node(node->left);
    if (node->hasRight) free_node(node->right);

    free(node);
}

void hufftree_destroy(huffnode *node) {
    if (node->hasLeft) free_node(node->left);
    if (node->hasRight) free_node(node->right);
}
