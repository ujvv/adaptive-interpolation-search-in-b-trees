#ifndef ADAPTIVE_INTERPOLATION_SEARCH_IN_B_TREES_BTREE_TEMPLATE_BTREE_TEMPLATEBIGNODE_HPP
#define ADAPTIVE_INTERPOLATION_SEARCH_IN_B_TREES_BTREE_TEMPLATE_BTREE_TEMPLATEBIGNODE_HPP


#include "btreenode_templateBigNode.hpp"
#include <cstdint>
#include <functional>

class BTreeTemplateBigNode {
public:
  friend class Tester;

  BTreeNodeTemplateBigNode *root = nullptr;

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
  BTreeLeafNodeTemplateBigNode *traverseToLeaf_templateBigNode(std::span<uint8_t> key);
};

// create a new tree and return a pointer to it
BTreeTemplateBigNode *btree_create_templateBigNode();

// destroy a tree created by btree_create_templateBigNode
void btree_destroy_templateBigNode(BTreeTemplateBigNode *);

// return true iff the key was present
bool btree_remove_templateBigNode(BTreeTemplateBigNode *tree, uint8_t *key, uint16_t keyLength);

// replaces exising record if any
void btree_insert_templateBigNode(BTreeTemplateBigNode *tree, uint8_t *key, uint16_t keyLength, uint8_t *value, uint16_t valueLength);

// returns a pointer to the associated value if present, nullptr otherwise
uint8_t *btree_lookup_templateBigNode(BTreeTemplateBigNode *tree, uint8_t *key, uint16_t keyLength, uint16_t &payloadLengthOut);

// invokes the callback for all records greater than or equal to key, in order.
// the key should be copied to keyOut before the call.
// the callback should be invoked with keyLength, value pointer, and value
// length iteration stops if there are no more keys or the callback returns
// false.
void btree_scan_templateBigNode(BTreeTemplateBigNode *tree, uint8_t *key, unsigned keyLength, uint8_t *keyOut, const std::function<bool(unsigned int, uint8_t *, unsigned int)> &found_callback);

#endif // ADAPTIVE_INTERPOLATION_SEARCH_IN_B_TREES_BTREE_TEMPLATE_BTREE_TEMPLATEBIGNODE_HPP
