#include "btree_binarySearch.hpp"
#include "btreenode_binarySearch.hpp"

inline std::vector<uint8_t> toByteVector(uint8_t *b, unsigned l) { return std::vector<uint8_t>(b, b + l); }

BTreeLeafNodeBinarySearch *BTreeBinarySearch::traverseToLeaf_binarySearch(std::span<uint8_t> key) {
  BTreeNodeBinarySearch *currentNode = root;
  while (!currentNode->isLeaf) {
    uint16_t childIndex = currentNode->getEntryIndexByKey(key);
    currentNode = reinterpret_cast<BTreeInnerNodeBinarySearch *>(currentNode)->getChild(childIndex);
  }
  return reinterpret_cast<BTreeLeafNodeBinarySearch *>(currentNode);
}

std::optional<std::span<uint8_t>> BTreeBinarySearch::lookup(std::span<uint8_t> key) {
  if (root == nullptr) {
    return std::nullopt;
  }
  BTreeLeafNodeBinarySearch *leaf = traverseToLeaf_binarySearch(key);
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

void BTreeBinarySearch::insert(std::span<uint8_t> key, std::span<uint8_t> value) {
  if (root == nullptr) {
    root = new BTreeLeafNodeBinarySearch();
  }
  auto toInsert = root->insert(key, value);
  if (!toInsert.has_value()) {
    return;
  }

  // We need to split the root
  BTreeNodeBinarySearch *nodeToInsert = toInsert->first;
  std::vector<uint8_t> splitKey = std::move(toInsert->second);
  BTreeInnerNodeBinarySearch *newRoot = new BTreeInnerNodeBinarySearch();
  newRoot->insertEntry(0, splitKey, root);
  newRoot->rightMostChildBinarySearch = nodeToInsert;
  root = newRoot;
}

void BTreeBinarySearch::destroy() {
  if (root != nullptr) {
    root->destroy();
  }
}

void BTreeBinarySearch::scan(std::span<uint8_t> key, uint8_t *keyOut, const std::function<bool(unsigned int, uint8_t *, unsigned int)> &found_callback) {
  if (root == nullptr) {
    return;
  }

  // Find the node that should contain the key
  BTreeLeafNodeBinarySearch *currentNode = traverseToLeaf_binarySearch(key);
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
    currentNode = currentNode->nextLeafNodeBinarySearch;
  }
}

std::vector<double> BTreeBinarySearch::analyzeLeafs() {
  std::vector<double> coefficientsOfVariation;
  if (root != nullptr) {
    BTreeNodeBinarySearch *currentNode = root;
    while (!currentNode->isLeaf) {
      currentNode = reinterpret_cast<BTreeInnerNodeBinarySearch *>(currentNode)->getChild(0);
    }
    while (currentNode != nullptr) {
      std::vector<uint32_t> keyDifferences = currentNode->calculateKeyDifferences();
      double mean = currentNode->mean(keyDifferences);
      double standardDeviation = currentNode->standardDeviation(keyDifferences, mean);
      double coefficient = standardDeviation / mean;
      coefficientsOfVariation.push_back(coefficient);
      currentNode = currentNode->nextLeafNodeBinarySearch;
    }
  }
  return coefficientsOfVariation;
}

std::vector<uint32_t> BTreeBinarySearch::numKeysLeafs() {
  std::vector<uint32_t> numKeysInLeafs;
  if (root != nullptr) {
    BTreeNodeBinarySearch *currentNode = root;
    while (!currentNode->isLeaf) {
      currentNode = reinterpret_cast<BTreeInnerNodeBinarySearch *>(currentNode)->getChild(0);
    }
    while (currentNode != nullptr) {
      uint32_t numKeysInCurrentLeaf = currentNode->numKeys;
      numKeysInLeafs.push_back(numKeysInCurrentLeaf);
      currentNode = currentNode->nextLeafNodeBinarySearch;
    }
  }
  return numKeysInLeafs;
}

std::vector<double> BTreeBinarySearch::analyzeInnerNodes() {
  std::vector<double> coefficientsOfVariation;
  if (root != nullptr) {
    root->analyzeInnerNodes(coefficientsOfVariation);
  }
  return coefficientsOfVariation;
}

std::vector<uint32_t> BTreeBinarySearch::numKeysInnerNodes() {
  std::vector<uint32_t> numKeysInInnerNodes;
  if (root != nullptr) {
    root->numKeysInnerNodes(numKeysInInnerNodes);
  }
  return numKeysInInnerNodes;
}

bool BTreeBinarySearch::remove(std::span<uint8_t> key) {
  if (root == nullptr) {
    return false;
  }
  return root->remove(key);
}

// create a new tree and return a pointer to it
BTreeBinarySearch *btree_create_binarySearch() { return new BTreeBinarySearch(); };

// destroy a tree created by btree_create_binarySearch
void btree_destroy_binarySearch(BTreeBinarySearch *t) {
  t->destroy();
  delete t;
}

// return true iff the key was present
bool btree_remove_binarySearch(BTreeBinarySearch *tree, uint8_t *key, uint16_t keyLength) { return tree->remove(std::span<uint8_t>(key, keyLength)); }

// replaces exising record if any
void btree_insert_binarySearch(BTreeBinarySearch *tree, uint8_t *key, uint16_t keyLength, uint8_t *value, uint16_t valueLength) { tree->insert(std::span<uint8_t>(key, keyLength), std::span<uint8_t>(value, valueLength)); }

// returns a pointer to the associated value if present, nullptr otherwise
uint8_t *btree_lookup_binarySearch(BTreeBinarySearch *tree, uint8_t *key, uint16_t keyLength, uint16_t &payloadLengthOut) {
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
void btree_scan_binarySearch(BTreeBinarySearch *tree, uint8_t *key, unsigned keyLength, uint8_t *keyOut, const std::function<bool(unsigned int, uint8_t *, unsigned int)> &found_callback) { return tree->scan(std::span<uint8_t>(key, keyLength), keyOut, found_callback); }

std::vector<double> btree_analyzeLeafs(BTreeBinarySearch *tree) {return tree->analyzeLeafs();}

std::vector<double> btree_analyzeInnerNodes(BTreeBinarySearch *tree) {return tree->analyzeInnerNodes();}

std::vector<uint32_t> btree_numKeysLeafs(BTreeBinarySearch *tree) {return tree->numKeysLeafs();}

std::vector<uint32_t> btree_numKeysInnerNodes(BTreeBinarySearch *tree) {return tree->numKeysInnerNodes();}