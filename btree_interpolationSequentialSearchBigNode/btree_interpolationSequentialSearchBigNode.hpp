#ifndef ADAPTIVE_INTERPOLATION_SEARCH_IN_B_TREES_BTREE_INTERPOLATIONSEQUENTIALSEARCHBIGNODE_BTREE_INTERPOLATIONSEQUENTIALSEARCHBIGNODE_HPP
#define ADAPTIVE_INTERPOLATION_SEARCH_IN_B_TREES_BTREE_INTERPOLATIONSEQUENTIALSEARCHBIGNODE_BTREE_INTERPOLATIONSEQUENTIALSEARCHBIGNODE_HPP


#include "btreenode_interpolationSequentialSearchBigNode.hpp"
#include <cstdint>
#include <functional>

class BTreeInterpolationSequentialSearchBigNode {
public:
  friend class Tester;

  BTreeNodeInterpolationSequentialSearchBigNode *root = nullptr;

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
  BTreeLeafNodeInterpolationSequentialSearchBigNode *traverseToLeaf_interpolationSequentialSearchBigNode(std::span<uint8_t> key);
};

// create a new tree and return a pointer to it
BTreeInterpolationSequentialSearchBigNode *btree_create_interpolationSequentialSearchBigNode();

// destroy a tree created by btree_create_interpolationSequentialSearchBigNode
void btree_destroy_interpolationSequentialSearchBigNode(BTreeInterpolationSequentialSearchBigNode *);

// return true iff the key was present
bool btree_remove_interpolationSequentialSearchBigNode(BTreeInterpolationSequentialSearchBigNode *tree, uint8_t *key, uint16_t keyLength);

// replaces exising record if any
void btree_insert_interpolationSequentialSearchBigNode(BTreeInterpolationSequentialSearchBigNode *tree, uint8_t *key, uint16_t keyLength, uint8_t *value, uint16_t valueLength);

// returns a pointer to the associated value if present, nullptr otherwise
uint8_t *btree_lookup_interpolationSequentialSearchBigNode(BTreeInterpolationSequentialSearchBigNode *tree, uint8_t *key, uint16_t keyLength, uint16_t &payloadLengthOut);

// invokes the callback for all records greater than or equal to key, in order.
// the key should be copied to keyOut before the call.
// the callback should be invoked with keyLength, value pointer, and value
// length iteration stops if there are no more keys or the callback returns
// false.
void btree_scan_interpolationSequentialSearchBigNode(BTreeInterpolationSequentialSearchBigNode *tree, uint8_t *key, unsigned keyLength, uint8_t *keyOut, const std::function<bool(unsigned int, uint8_t *, unsigned int)> &found_callback);

#endif // ADAPTIVE_INTERPOLATION_SEARCH_IN_B_TREES_BTREE_INTERPOLATIONSEQUENTIALSEARCHBIGNODE_BTREE_INTERPOLATIONSEQUENTIALSEARCHBIGNODE_HPP
