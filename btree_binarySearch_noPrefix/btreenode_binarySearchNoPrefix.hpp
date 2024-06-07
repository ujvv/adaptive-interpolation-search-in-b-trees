#ifndef ADAPTIVE_INTERPOLATION_SEARCH_IN_B_TREES_BTREE_BINARYSEARCH_NOPREFIX_BTREENODE_BINARYSEARCHNOPREFIX_HPP
#define ADAPTIVE_INTERPOLATION_SEARCH_IN_B_TREES_BTREE_BINARYSEARCH_NOPREFIX_BTREENODE_BINARYSEARCHNOPREFIX_HPP

#include <cstdint>
#include <cstring>
#include <functional>
#include <optional>
#include <span>
#include <string>


constexpr uint32_t NODE_SIZE_BINARYSEARCHNOPREFIX = 4096;
// constexpr uint32_t NODE_SIZE_BINARYSEARCHNOPREFIX = 8192;

// Forward declarations
class BTreeNodeBinarySearchNoPrefix;
class BTreeLeafNodeBinarySearchNoPrefix;

struct PageSlotBinarySearchNoPrefix {
  uint16_t offset;      // Offset on the heap to the key-value entry
  uint16_t keyLength;   // Length of the key on the heap
  uint16_t valueLength; // Length of the value on the heap
  uint32_t keyHead;     // First 4 bytes of the key
} __attribute__((packed));

struct BTreeNodeHeaderBinarySearchNoPrefix {
  union {
    BTreeNodeBinarySearchNoPrefix *rightMostChildBinarySearchNoPrefix;   // Pointer to the rightmost child if it's a inner node
    BTreeLeafNodeBinarySearchNoPrefix *nextLeafNodeBinarySearchNoPrefix; // Pointer to the next leaf node if it's a leaf node
  };
  bool isLeaf;
  uint16_t numKeys = 0;
  uint16_t spaceUsed = 0;
  uint16_t freeOffset = NODE_SIZE_BINARYSEARCHNOPREFIX - sizeof(BTreeNodeHeaderBinarySearchNoPrefix);
  std::array<uint32_t, 16> hints = {0};
};

class alignas(NODE_SIZE_BINARYSEARCHNOPREFIX) BTreeNodeBinarySearchNoPrefix : public BTreeNodeHeaderBinarySearchNoPrefix {
public:
  friend class BTreeBinarySearchNoPrefix;
  friend class Tester;
  static constexpr uint32_t CONTENT_SIZE = NODE_SIZE_BINARYSEARCHNOPREFIX - sizeof(BTreeNodeHeaderBinarySearchNoPrefix);
  uint8_t content[CONTENT_SIZE];

  BTreeNodeBinarySearchNoPrefix() = default;

  // Returns the key at a given positon
  std::span<uint8_t> getShortenedKey(uint16_t position) {
    auto slot = reinterpret_cast<PageSlotBinarySearchNoPrefix *>(content + position * sizeof(PageSlotBinarySearchNoPrefix));
    return std::span<uint8_t>(content + slot->offset, slot->keyLength);
  }

  // Returns the key at a given position
  std::vector<uint8_t> getFullKey(uint16_t position) {
    auto slot = reinterpret_cast<PageSlotBinarySearchNoPrefix *>(content + position * sizeof(PageSlotBinarySearchNoPrefix));
    std::vector<uint8_t> key(slot->keyLength);
    std::memcpy(key.data(), content + slot->offset, slot->keyLength);
    return key;
  }
  // Utility functions to retrieve the keys of the node in different represantations

  std::vector<std::vector<uint8_t>> getKeys();
  std::vector<std::string> getKeysAsString();
  std::vector<std::string> getShortenedKeysAsString();

private:
  // Returns the index where this key is located or should be inserted. In a leaf, this is directly the index
  // Where the key should be located / inserted, otherwise it returns the index of the child where the key should be located / inserted
  uint16_t getEntryIndexByKey(std::span<uint8_t> key);

  // Inserts the given key and value into the subtree of this node
  // If the return value is not empty, it contains the new child and the splitkey which needs to be inserted in the caller
  std::optional<std::pair<BTreeNodeBinarySearchNoPrefix *, std::vector<uint8_t>>> insert(std::span<uint8_t> key, std::span<uint8_t> value);

  // Removes the given key from the subtree of this node, returns true if the key was found and removed
  bool remove(std::span<uint8_t> key);

  // Destroys the subtree of this node
  void destroy();

  // Returns the index where the node should be split such that both the resulting nodes have an equal amount of free space
  // The splitIndex should remain in the current node
  uint16_t getSplitIndex();

  // Reorders the entries on the heap such that new elements can be inserted
  void compact();

  // Inserts the given key and value at the given position
  void insertEntry(uint16_t position, std::span<uint8_t> key, std::span<uint8_t> value);

  // Inserts the given key and childPointer at the given position
  void insertEntry(uint16_t position, std::span<uint8_t> key, BTreeNodeBinarySearchNoPrefix *childPointer);

  // Erases the entry at the given position
  void eraseEntry(uint16_t position);

  // Checks if the given key is smaller or equal that the key at the given position, the given key should not contain the prefix
  bool keySmallerEqualThanAtPosition(uint16_t position, uint32_t keyHead, std::span<uint8_t> key);

  // Checks if the given key is greater that the key at the given position, the given key should not contain the prefix
  bool keyLargerThanAtPosition(uint16_t position, uint32_t keyHead, std::span<uint8_t> key) { return !keySmallerEqualThanAtPosition(position, keyHead, key); }

  // Splits the node at the given position and returns the new node
  BTreeNodeBinarySearchNoPrefix *splitNode(uint16_t splitIndex);
};

class BTreeInnerNodeBinarySearchNoPrefix : public BTreeNodeBinarySearchNoPrefix {
public:
  BTreeInnerNodeBinarySearchNoPrefix() { isLeaf = false; }

  // Returns the child pointer at the given position
  BTreeNodeBinarySearchNoPrefix *getChild(uint16_t position) {
    // the rightmost child is accessed
    if (position == numKeys) {
      return rightMostChildBinarySearchNoPrefix;
    }
    auto slot = reinterpret_cast<PageSlotBinarySearchNoPrefix *>(content + position * sizeof(PageSlotBinarySearchNoPrefix));
    BTreeNodeBinarySearchNoPrefix *pointer = nullptr;
    std::memcpy(&pointer, content + slot->offset + slot->keyLength, sizeof(BTreeNodeBinarySearchNoPrefix *));
    return pointer;
  }

  // Overwrites the child pointer with a new pointer at a given position, the key won't be changed
  void overwriteChild(uint16_t position, BTreeNodeBinarySearchNoPrefix *newChild);
};

class BTreeLeafNodeBinarySearchNoPrefix : public BTreeNodeBinarySearchNoPrefix {
public:
  BTreeLeafNodeBinarySearchNoPrefix() {
    isLeaf = true;
    nextLeafNodeBinarySearchNoPrefix = nullptr;
  }

  // Returns the value at the given position
  std::span<uint8_t> getValue(uint16_t position) {
    auto slot = reinterpret_cast<PageSlotBinarySearchNoPrefix *>(content + position * sizeof(PageSlotBinarySearchNoPrefix));
    return std::span<uint8_t>(content + slot->offset + slot->keyLength, slot->valueLength);
  }
};

#endif // ADAPTIVE_INTERPOLATION_SEARCH_IN_B_TREES_BTREE_BINARYSEARCH_NOPREFIX_BTREENODE_BINARYSEARCHNOPREFIX_HPP