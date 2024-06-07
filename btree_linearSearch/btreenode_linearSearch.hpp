#ifndef ADAPTIVE_INTERPOLATION_SEARCH_IN_B_TREES_BTREE_LINEARSEARCH_BTREENODE_LINEARSEARCH_HPP
#define ADAPTIVE_INTERPOLATION_SEARCH_IN_B_TREES_BTREE_LINEARSEARCH_BTREENODE_LINEARSEARCH_HPP

#include <cstdint>
#include <cstring>
#include <functional>
#include <optional>
#include <span>
#include <string>


constexpr uint32_t NODE_SIZE_LINEARSEARCH = 4096;
// constexpr uint32_t NODE_SIZE_LINEARSEARCH = 8192;

// Forward declarations
class BTreeNodeLinearSearch;
class BTreeLeafNodeLinearSearch;

struct PageSlotLinearSearch {
  uint16_t offset;      // Offset on the heap to the key-value entry
  uint16_t keyLength;   // Length of the key on the heap
  uint16_t valueLength; // Length of the value on the heap
  uint32_t keyHead;     // First 4 bytes of the key
} __attribute__((packed));

struct FenceKeySlotLinearSearch {
  uint16_t offset; // Offset of the key on the heap
  uint16_t len;    // Length of the key on the heap
};

struct BTreeNodeHeaderLinearSearch {
  union {
    BTreeNodeLinearSearch *rightMostChildLinearSearch;   // Pointer to the rightmost child if it's a inner node
    BTreeLeafNodeLinearSearch *nextLeafNodeLinearSearch; // Pointer to the next leaf node if it's a leaf node
  };
  bool isLeaf;
  uint16_t numKeys = 0;
  uint16_t spaceUsed = 0;
  uint16_t freeOffset = NODE_SIZE_LINEARSEARCH - sizeof(BTreeNodeHeaderLinearSearch);
  uint16_t prefixLen = 0;
  uint16_t prefixOffset = 0;
  FenceKeySlotLinearSearch lowerFence = {0, 0};
  FenceKeySlotLinearSearch upperFence = {0, 0};
  std::array<uint32_t, 16> hints = {0};
};

class alignas(NODE_SIZE_LINEARSEARCH) BTreeNodeLinearSearch : public BTreeNodeHeaderLinearSearch {
public:
  friend class BTreeLinearSearch;
  friend class Tester;
  static constexpr uint32_t CONTENT_SIZE = NODE_SIZE_LINEARSEARCH - sizeof(BTreeNodeHeaderLinearSearch);
  uint8_t content[CONTENT_SIZE];

  BTreeNodeLinearSearch() = default;

  BTreeNodeLinearSearch(std::span<uint8_t> lowerFenceKey, std::span<uint8_t> upperFenceKey);

  // Returns the key without the prefix at a given positon
  std::span<uint8_t> getShortenedKey(uint16_t position) {
    auto slot = reinterpret_cast<PageSlotLinearSearch *>(content + position * sizeof(PageSlotLinearSearch));
    return std::span<uint8_t>(content + slot->offset, slot->keyLength);
  }

  // Returns the full key at a given position
  std::vector<uint8_t> getFullKey(uint16_t position) {
    auto slot = reinterpret_cast<PageSlotLinearSearch *>(content + position * sizeof(PageSlotLinearSearch));
    std::vector<uint8_t> key(slot->keyLength + prefixLen);

    if (prefixLen > 0) {
      std::memcpy(key.data(), content + prefixOffset, prefixLen);
    }
    if (slot->keyLength > 0) {
      std::memcpy(key.data() + prefixLen, content + slot->offset, slot->keyLength);
    }
    return key;
  }

  std::vector<uint8_t> getLowerFenceKey() { return getFenceKey(lowerFence); }

  std::vector<uint8_t> getUpperFenceKey() { return getFenceKey(upperFence); }

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
  std::optional<std::pair<BTreeNodeLinearSearch *, std::vector<uint8_t>>> insert(std::span<uint8_t> key, std::span<uint8_t> value);

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
  void insertEntry(uint16_t position, std::span<uint8_t> key, BTreeNodeLinearSearch *childPointer);

  // Erases the entry at the given position
  void eraseEntry(uint16_t position);

  // Checks if the given key is smaller or equal that the key at the given position, the given key should not contain the prefix
  bool keySmallerEqualThanAtPosition(uint16_t position, uint32_t keyHead, std::span<uint8_t> key);

  // Checks if the given key is greater that the key at the given position, the given key should not contain the prefix
  bool keyLargerThanAtPosition(uint16_t position, uint32_t keyHead, std::span<uint8_t> key) { return !keySmallerEqualThanAtPosition(position, keyHead, key); }

  // Splits the node at the given position and returns the new node
  BTreeNodeLinearSearch *splitNode(uint16_t splitIndex, std::span<uint8_t> splitKey);

  std::vector<uint8_t> getFenceKey(FenceKeySlotLinearSearch &fenceKey) {
    std::vector<uint8_t> key(prefixLen + fenceKey.len);
    if (prefixLen > 0) {
      std::memcpy(key.data(), content + prefixOffset, prefixLen);
    }
    if (fenceKey.len > 0) {
      std::memcpy(key.data() + prefixLen, content + fenceKey.offset, fenceKey.len);
    }
    return key;
  }
};

class BTreeInnerNodeLinearSearch : public BTreeNodeLinearSearch {
public:
  BTreeInnerNodeLinearSearch(std::span<uint8_t> lowerFenceKey, std::span<uint8_t> upperFenceKey) : BTreeNodeLinearSearch(lowerFenceKey, upperFenceKey) { isLeaf = false; }

  BTreeInnerNodeLinearSearch() { isLeaf = false; }

  // Returns the child pointer at the given position
  BTreeNodeLinearSearch *getChild(uint16_t position) {
    // the rightmost child is accessed
    if (position == numKeys) {
      return rightMostChildLinearSearch;
    }
    auto slot = reinterpret_cast<PageSlotLinearSearch *>(content + position * sizeof(PageSlotLinearSearch));
    BTreeNodeLinearSearch *pointer = nullptr;
    std::memcpy(&pointer, content + slot->offset + slot->keyLength, sizeof(BTreeNodeLinearSearch *));
    return pointer;
  }

  // Overwrites the child pointer with a new pointer at a given position, the key won't be changed
  void overwriteChild(uint16_t position, BTreeNodeLinearSearch *newChild);
};

class BTreeLeafNodeLinearSearch : public BTreeNodeLinearSearch {
public:
  BTreeLeafNodeLinearSearch(std::span<uint8_t> lowerFenceKey, std::span<uint8_t> upperFenceKey) : BTreeNodeLinearSearch(lowerFenceKey, upperFenceKey) { isLeaf = true; }

  BTreeLeafNodeLinearSearch() {
    isLeaf = true;
    nextLeafNodeLinearSearch = nullptr;
  }

  // Returns the value at the given position
  std::span<uint8_t> getValue(uint16_t position) {
    auto slot = reinterpret_cast<PageSlotLinearSearch *>(content + position * sizeof(PageSlotLinearSearch));
    return std::span<uint8_t>(content + slot->offset + slot->keyLength, slot->valueLength);
  }
};

#endif // ADAPTIVE_INTERPOLATION_SEARCH_IN_B_TREES_BTREE_LINEARSEARCH_BTREENODE_LINEARSEARCH_HPP