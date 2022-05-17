#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#define CHAR_SIZE 8
#define CODE_LENGTH_COUNT 16

typedef struct huff_node {
    struct huff_node *left;
    struct huff_node *right;
    bool hasLeft;
    bool hasRight;
    uint8_t symbol;
} huff_node;

typedef struct huff_tree {
    huff_node *root;
    uint8_t class;
    uint8_t id;
} huff_tree;

static huff_node *create_huff_node() {
    return calloc(1, sizeof(huff_node));
}

static void add_symbol_to_huff_node(huff_node *root, uint16_t code, int codeLength, uint8_t symbol) {
    huff_node *curr = root;

    for (int i=codeLength; 0 < i; --i) {
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
}

huff_tree *huf_parse_huff_tree(uint8_t *data, size_t *offset) {
    size_t initialOffset = *offset;

    huff_node *root = calloc(1, sizeof(huff_node));

    huff_tree *tree = malloc(sizeof *tree);
    tree->root = root;
    tree->class = (data[initialOffset] & 0x10) >> 4;
    tree->id = data[initialOffset] & 0x1;

    *offset += CODE_LENGTH_COUNT + 1; // move offset after the code lengths

    int codeValue = 0;
    for (size_t codeLength = 1; codeLength <= CODE_LENGTH_COUNT; ++codeLength) {
        int codeCount = data[initialOffset + codeLength];

        for (size_t i = 0; i < codeCount; ++i) {
            uint16_t code = codeValue++;
            uint8_t symbol = data[(*offset)++];

            add_symbol_to_huff_node(root, code, codeLength, symbol);
        }

        codeValue <<= 1;
    }

    return tree;
}

static void free_huff_node(huff_node *node) {
    if (node->hasLeft) {
        free_huff_node(node->left);
    }

    if (node->hasRight) {
        free_huff_node(node->right);
    }

    free(node);
}

void huf_free_huff_tree(huff_tree *tree) {
    free_huff_node(tree->root);
    free(tree);
}

static void print_huff_node(huff_node *node, int depth) {
    if (node->hasLeft) {
        print_huff_node(node->left, depth + 1);
    }

    for (int i=0; i < depth; ++i) {
        printf("  ");
    }
    printf("%02X\n", node->symbol);

    if (node->hasRight) {
        print_huff_node(node->right, depth + 1);
    }
}

void huf_print_huff_tree(huff_tree *tree) {
    print_huff_node(tree->root, 0);
}


uint8_t huf_get_huff_tree_class(huff_tree *tree) {
    return tree->class;
}

uint8_t huf_get_huff_tree_id(huff_tree *tree) {
    return tree->id;
}