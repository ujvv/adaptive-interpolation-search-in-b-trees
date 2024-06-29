#include "btree_interpolationSequentialSearch.hpp"
#include "btreenode_interpolationSequentialSearch.hpp"

inline std::vector<uint8_t> toByteVector(uint8_t *b, unsigned l) { return std::vector<uint8_t>(b, b + l); }

BTreeLeafNodeInterpolationSequentialSearch *BTreeInterpolationSequentialSearch::traverseToLeaf_interpolationSequentialSearch(std::span<uint8_t> key) {
  BTreeNodeInterpolationSequentialSearch *currentNode = root;
  while (!currentNode->isLeaf) {
    uint16_t childIndex = currentNode->getEntryIndexByKey(key);
    currentNode = reinterpret_cast<BTreeInnerNodeInterpolationSequentialSearch *>(currentNode)->getChild(childIndex);
  }
  return reinterpret_cast<BTreeLeafNodeInterpolationSequentialSearch *>(currentNode);
}

std::optional<std::span<uint8_t>> BTreeInterpolationSequentialSearch::lookup(std::span<uint8_t> key) {
  if (root == nullptr) {
    return std::nullopt;
  }
  BTreeLeafNodeInterpolationSequentialSearch *leaf = traverseToLeaf_interpolationSequentialSearch(key);
  uint16_t entryIndex = leaf->getEntryIndexByKey(key);
  if (entryIndex < leaf->numKeys) {
    auto existingKey = leaf->getFullKey(entryIndex);
    bool keysEqual = key.size() == existingKey.size() && std::equal(key.begin(), key.end(), existingKey.begin());
    if (keysEqual) {
      return leaf->getValue(entryIndex);
    }
  }
  return std::nullopt;
}

void BTreeInterpolationSequentialSearch::insert(std::span<uint8_t> key, std::span<uint8_t> value) {
  if (root == nullptr) {
    root = new BTreeLeafNodeInterpolationSequentialSearch();
  }
  auto toInsert = root->insert(key, value);
  if (!toInsert.has_value()) {
    return;
  }

  // We need to split the root
  BTreeNodeInterpolationSequentialSearch *nodeToInsert = toInsert->first;
  std::vector<uint8_t> splitKey = std::move(toInsert->second);
  BTreeInnerNodeInterpolationSequentialSearch *newRoot = new BTreeInnerNodeInterpolationSequentialSearch();
  newRoot->insertEntry(0, splitKey, root);
  newRoot->rightMostChildInterpolationSequentialSearch = nodeToInsert;
  root = newRoot;
}

void BTreeInterpolationSequentialSearch::destroy() {
  if (root != nullptr) {
    root->destroy();
  }
}

void BTreeInterpolationSequentialSearch::scan(std::span<uint8_t> key, uint8_t *keyOut, const std::function<bool(unsigned int, uint8_t *, unsigned int)> &found_callback) {
  if (root == nullptr) {
    return;
  }

  // Find the node that should contain the key
  BTreeLeafNodeInterpolationSequentialSearch *currentNode = traverseToLeaf_interpolationSequentialSearch(key);
  std::vector<uint8_t> keyVector(key.begin(), key.end());
  bool currentKeyValid = false;
  bool continueIteration = true;

  while (currentNode != nullptr && continueIteration) {
    // Get all keys in once, this should be more efficient
    std::vector<std::vector<uint8_t>> keys = currentNode->getKeys();
    for (uint16_t i = 0; i < keys.size() && continueIteration; i++) {
      // Check if the key is valid
      if (!currentKeyValid && keys[i] >= keyVector) {
        currentKeyValid = true;
      }

      if (currentKeyValid) {
        std::span<uint8_t> valueSpan = currentNode->getValue(i);
        // Copy key
        if (keys[i].size() > 0) {
          std::memcpy(keyOut, keys[i].data(), keys[i].size());
        }
        // Invoke callback
        continueIteration &= found_callback(keys[i].size(), valueSpan.data(), valueSpan.size());
      }
    }
    currentNode = currentNode->nextLeafNodeInterpolationSequentialSearch;
  }
}

bool BTreeInterpolationSequentialSearch::remove(std::span<uint8_t> key) {
  if (root == nullptr) {
    return false;
  }
  return root->remove(key);
}

// create a new tree and return a pointer to it
BTreeInterpolationSequentialSearch *btree_create_interpolationSequentialSearch() { return new BTreeInterpolationSequentialSearch(); };

// destroy a tree created by btree_create_interpolationSequentialSearch
void btree_destroy_interpolationSequentialSearch(BTreeInterpolationSequentialSearch *t) {
  t->destroy();
  delete t;
}

// return true iff the key was present
bool btree_remove_interpolationSequentialSearch(BTreeInterpolationSequentialSearch *tree, uint8_t *key, uint16_t keyLength) { return tree->remove(std::span<uint8_t>(key, keyLength)); }

// replaces exising record if any
void btree_insert_interpolationSequentialSearch(BTreeInterpolationSequentialSearch *tree, uint8_t *key, uint16_t keyLength, uint8_t *value, uint16_t valueLength) { tree->insert(std::span<uint8_t>(key, keyLength), std::span<uint8_t>(value, valueLength)); }

// returns a pointer to the associated value if present, nullptr otherwise
uint8_t *btree_lookup_interpolationSequentialSearch(BTreeInterpolationSequentialSearch *tree, uint8_t *key, uint16_t keyLength, uint16_t &payloadLengthOut) {
  auto value = tree->lookup(std::span<uint8_t>(key, keyLength));
  if (!value.has_value()) {
    return nullptr;
  }
  payloadLengthOut = value->size();
  return value->data();
}

// invokes the callback for all records greater than or equal to key, in order.
// the key should be copied to keyOut before the call.
// the callback should be invoked with keyLength, value pointer, and value
// length iteration stops if there are no more keys or the callback returns
// false.
void btree_scan_interpolationSequentialSearch(BTreeInterpolationSequentialSearch *tree, uint8_t *key, unsigned keyLength, uint8_t *keyOut, const std::function<bool(unsigned int, uint8_t *, unsigned int)> &found_callback) { return tree->scan(std::span<uint8_t>(key, keyLength), keyOut, found_callback); }
