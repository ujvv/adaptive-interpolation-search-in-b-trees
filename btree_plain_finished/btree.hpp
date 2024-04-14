#ifndef EYTZINGER_LAYOUT_FOR_B_TREE_NODES_BTREE_PLAIN_FINISHED_BTREE_HPP
#define EYTZINGER_LAYOUT_FOR_B_TREE_NODES_BTREE_PLAIN_FINISHED_BTREE_HPP

#include <cstdint>
#include <functional>

/*class BTree {
     BTreeNode *root;

    BTree() {
        root = new BTreeNode();
        root->is_leaf = true;
    }
};*/

struct BTree;

// create a new tree and return a pointer to it
BTree *btree_create();

// destroy a tree created by btree_create
void btree_destroy(BTree *);

// return true iff the key was present
bool btree_remove(BTree *tree, uint8_t *key, uint16_t keyLength);

// replaces exising record if any
void btree_insert(BTree *tree, uint8_t *key, uint16_t keyLength, uint8_t *value,
                  uint16_t valueLength);

// returns a pointer to the associated value if present, nullptr otherwise
uint8_t *btree_lookup(BTree *tree, uint8_t *key, uint16_t keyLength,
                      uint16_t &payloadLengthOut);

// invokes the callback for all records greater than or equal to key, in order.
// the key should be copied to keyOut before the call.
// the callback should be invoked with keyLength, value pointer, and value
// length iteration stops if there are no more keys or the callback returns
// false.
void btree_scan(BTree *tree, uint8_t *key, unsigned keyLength, uint8_t *keyOut,
                const std::function<bool(unsigned int, uint8_t *, unsigned int)>
                    &found_callback);

#endif // EYTZINGER_LAYOUT_FOR_B_TREE_NODES_BTREE_PLAIN_FINISHED_BTREE_HPP
