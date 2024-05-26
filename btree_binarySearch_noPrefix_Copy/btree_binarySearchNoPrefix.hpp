#ifndef EYTZINGER_LAYOUT_FOR_B_TREE_NODES_BTREE_BINARYSEARCH_NOPREFIX_BTREE_BINARYSEARCHNOPREFIX_HPP
#define EYTZINGER_LAYOUT_FOR_B_TREE_NODES_BTREE_BINARYSEARCH_NOPREFIX_BTREE_BINARYSEARCHNOPREFIX_HPP


#include "btreenode_binarySearchNoPrefix.hpp"
#include <cstdint>
#include <functional>

class BTreeBinarySearchNoPrefix {
public:
  friend class Tester;

  BTreeNodeBinarySearchNoPrefix *root = nullptr;

  // Insert the key and value into the tree
  void insert(std::span<uint8_t> key, std::span<uint8_t> value);

  // Remove the key from the tree
  bool remove(std::span<uint8_t> key);

  // Lookup the value corresponding to the given key in the tree
  std::optional<std::span<uint8_t>> lookup(std::span<uint8_t> key);

  // Scan the tree for all keys greater than or equal to the given key as long as the callback returns true
  void scan(std::span<uint8_t> key, uint8_t *keyOut, const std::function<bool(unsigned int, uint8_t *, unsigned int)> &found_callback);

  // Destroy the tree
  void destroy();

private:
  // Traverse the tree to find the leaf node that should contain the given key
  BTreeLeafNodeBinarySearchNoPrefix *traverseToLeaf_binarySearchNoPrefix(std::span<uint8_t> key);
};

// create a new tree and return a pointer to it
BTreeBinarySearchNoPrefix *btree_create_binarySearchNoPrefix();

// destroy a tree created by btree_create_binarySearchNoPrefix
void btree_destroy_binarySearchNoPrefix(BTreeBinarySearchNoPrefix *);

// return true iff the key was present
bool btree_remove_binarySearchNoPrefix(BTreeBinarySearchNoPrefix *tree, uint8_t *key, uint16_t keyLength);

// replaces exising record if any
void btree_insert_binarySearchNoPrefix(BTreeBinarySearchNoPrefix *tree, uint8_t *key, uint16_t keyLength, uint8_t *value, uint16_t valueLength);

// returns a pointer to the associated value if present, nullptr otherwise
uint8_t *btree_lookup_binarySearchNoPrefix(BTreeBinarySearchNoPrefix *tree, uint8_t *key, uint16_t keyLength, uint16_t &payloadLengthOut);

// invokes the callback for all records greater than or equal to key, in order.
// the key should be copied to keyOut before the call.
// the callback should be invoked with keyLength, value pointer, and value
// length iteration stops if there are no more keys or the callback returns
// false.
void btree_scan_binarySearchNoPrefix(BTreeBinarySearchNoPrefix *tree, uint8_t *key, unsigned keyLength, uint8_t *keyOut, const std::function<bool(unsigned int, uint8_t *, unsigned int)> &found_callback);

#endif // EYTZINGER_LAYOUT_FOR_B_TREE_NODES_BTREE_BINARYSEARCH_NOPREFIX_BTREE_BINARYSEARCHNOPREFIX_HPP
