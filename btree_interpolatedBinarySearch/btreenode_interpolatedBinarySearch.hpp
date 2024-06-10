#ifndef ADAPTIVE_INTERPOLATION_SEARCH_IN_B_TREES_BTREE_INTERPOLATEDBINARYSEARCH_BTREENODE_INTERPOLATEDBINARYSEARCH_HPP
#define ADAPTIVE_INTERPOLATION_SEARCH_IN_B_TREES_BTREE_INTERPOLATEDBINARYSEARCH_BTREENODE_INTERPOLATEDBINARYSEARCH_HPP

#include <cstdint>
#include <cstring>
#include <functional>
#include <optional>
#include <span>
#include <string>


constexpr uint32_t NODE_SIZE_INTERPOLATEDBINARYSEARCH = 4096;
// constexpr uint32_t NODE_SIZE_INTERPOLATEDBINARYSEARCH = 8192;


// Forward declarations
class BTreeNodeInterpolatedBinarySearch;
class BTreeLeafNodeInterpolatedBinarySearch;

struct PageSlotInterpolatedBinarySearch {
  uint16_t offset;      // Offset on the heap to the key-value entry
  uint16_t keyLength;   // Length of the key on the heap
  uint16_t valueLength; // Length of the value on the heap
  uint32_t keyHead;     // First 4 bytes of the key
} __attribute__((packed));

struct FenceKeySlotInterpolatedBinarySearch {
  uint16_t offset; // Offset of the key on the heap
  uint16_t len;    // Length of the key on the heap
};

struct BTreeNodeHeaderInterpolatedBinarySearch {
  union {
    BTreeNodeInterpolatedBinarySearch *rightMostChildInterpolatedBinarySearch;   // Pointer to the rightmost child if it's a inner node
    BTreeLeafNodeInterpolatedBinarySearch *nextLeafNodeInterpolatedBinarySearch; // Pointer to the next leaf node if it's a leaf node
  };
  bool isLeaf;
  uint16_t numKeys = 0;
  uint16_t spaceUsed = 0;
  uint16_t freeOffset = NODE_SIZE_INTERPOLATEDBINARYSEARCH - sizeof(BTreeNodeHeaderInterpolatedBinarySearch);
  uint16_t prefixLen = 0;
  uint16_t prefixOffset = 0;
  FenceKeySlotInterpolatedBinarySearch lowerFence = {0, 0};
  FenceKeySlotInterpolatedBinarySearch upperFence = {0, 0};
  std::array<uint32_t, 16> hints = {0};
};

class alignas(NODE_SIZE_INTERPOLATEDBINARYSEARCH) BTreeNodeInterpolatedBinarySearch : public BTreeNodeHeaderInterpolatedBinarySearch {
public:
  friend class BTreeInterpolatedBinarySearch;
  friend class Tester;
  static constexpr uint32_t CONTENT_SIZE = NODE_SIZE_INTERPOLATEDBINARYSEARCH - sizeof(BTreeNodeHeaderInterpolatedBinarySearch);
  uint8_t content[CONTENT_SIZE];

  BTreeNodeInterpolatedBinarySearch() = default;

  BTreeNodeInterpolatedBinarySearch(std::span<uint8_t> lowerFenceKey, std::span<uint8_t> upperFenceKey);

  // Returns the key without the prefix at a given positon
  std::span<uint8_t> getShortenedKey(uint16_t position) {
    auto slot = reinterpret_cast<PageSlotInterpolatedBinarySearch *>(content + position * sizeof(PageSlotInterpolatedBinarySearch));
    return std::span<uint8_t>(content + slot->offset, slot->keyLength);
  }

  // Returns the full key at a given position
  std::vector<uint8_t> getFullKey(uint16_t position) {
    auto slot = reinterpret_cast<PageSlotInterpolatedBinarySearch *>(content + position * sizeof(PageSlotInterpolatedBinarySearch));
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
  std::optional<std::pair<BTreeNodeInterpolatedBinarySearch *, std::vector<uint8_t>>> insert(std::span<uint8_t> key, std::span<uint8_t> value);

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
  void insertEntry(uint16_t position, std::span<uint8_t> key, BTreeNodeInterpolatedBinarySearch *childPointer);

  // Erases the entry at the given position
  void eraseEntry(uint16_t position);

  // Checks if the given key is smaller or equal that the key at the given position, the given key should not contain the prefix
  bool keySmallerEqualThanAtPosition(uint16_t position, uint32_t keyHead, std::span<uint8_t> key);

  // Checks if the given key is smaller or equal that the key at the given position, the given key should not contain the prefix
  bool keySmallerThanAtPosition(uint16_t position, uint32_t keyHead, std::span<uint8_t> key);

  // Checks if the given key is greater that the key at the given position, the given key should not contain the prefix
  bool keyLargerThanAtPosition(uint16_t position, uint32_t keyHead, std::span<uint8_t> key) { return !keySmallerEqualThanAtPosition(position, keyHead, key); }

  // Splits the node at the given position and returns the new node
  BTreeNodeInterpolatedBinarySearch *splitNode(uint16_t splitIndex, std::span<uint8_t> splitKey);

  std::vector<uint8_t> getFenceKey(FenceKeySlotInterpolatedBinarySearch &fenceKey) {
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

class BTreeInnerNodeInterpolatedBinarySearch : public BTreeNodeInterpolatedBinarySearch {
public:
  BTreeInnerNodeInterpolatedBinarySearch(std::span<uint8_t> lowerFenceKey, std::span<uint8_t> upperFenceKey) : BTreeNodeInterpolatedBinarySearch(lowerFenceKey, upperFenceKey) { isLeaf = false; }

  BTreeInnerNodeInterpolatedBinarySearch() { isLeaf = false; }

  // Returns the child pointer at the given position
  BTreeNodeInterpolatedBinarySearch *getChild(uint16_t position) {
    // the rightmost child is accessed
    if (position == numKeys) {
      return rightMostChildInterpolatedBinarySearch;
    }
    auto slot = reinterpret_cast<PageSlotInterpolatedBinarySearch *>(content + position * sizeof(PageSlotInterpolatedBinarySearch));
    BTreeNodeInterpolatedBinarySearch *pointer = nullptr;
    std::memcpy(&pointer, content + slot->offset + slot->keyLength, sizeof(BTreeNodeInterpolatedBinarySearch *));
    return pointer;
  }

  // Overwrites the child pointer with a new pointer at a given position, the key won't be changed
  void overwriteChild(uint16_t position, BTreeNodeInterpolatedBinarySearch *newChild);
};

class BTreeLeafNodeInterpolatedBinarySearch : public BTreeNodeInterpolatedBinarySearch {
public:
  BTreeLeafNodeInterpolatedBinarySearch(std::span<uint8_t> lowerFenceKey, std::span<uint8_t> upperFenceKey) : BTreeNodeInterpolatedBinarySearch(lowerFenceKey, upperFenceKey) { isLeaf = true; }

  BTreeLeafNodeInterpolatedBinarySearch() {
    isLeaf = true;
    nextLeafNodeInterpolatedBinarySearch = nullptr;
  }

  // Returns the value at the given position
  std::span<uint8_t> getValue(uint16_t position) {
    auto slot = reinterpret_cast<PageSlotInterpolatedBinarySearch *>(content + position * sizeof(PageSlotInterpolatedBinarySearch));
    return std::span<uint8_t>(content + slot->offset + slot->keyLength, slot->valueLength);
  }
};

#endif // ADAPTIVE_INTERPOLATION_SEARCH_IN_B_TREES_BTREE_INTERPOLATEDBINARYSEARCH_BTREENODE_INTERPOLATEDBINARYSEARCH_HPP