#ifndef ADAPTIVE_INTERPOLATION_SEARCH_IN_B_TREES_BTREE_INTERPOLATIONSEQUENTIALSEARCHBIGNODE_BTREENODE_INTERPOLATIONSEQUENTIALSEARCHBIGNODE_HPP
#define ADAPTIVE_INTERPOLATION_SEARCH_IN_B_TREES_BTREE_INTERPOLATIONSEQUENTIALSEARCHBIGNODE_BTREENODE_INTERPOLATIONSEQUENTIALSEARCHBIGNODE_HPP

#include <cstdint>
#include <cstring>
#include <functional>
#include <optional>
#include <span>
#include <string>

constexpr uint32_t NODE_SIZE_INTERPOLATIONSEQUENTIALSEARCHBIGNODE = 65536;
// constexpr uint32_t NODE_SIZE_INTERPOLATIONSEQUENTIALSEARCHBIGNODE = 131072;

// Forward declarations
class BTreeNodeInterpolationSequentialSearchBigNode;
class BTreeLeafNodeInterpolationSequentialSearchBigNode;

struct PageSlotInterpolationSequentialSearchBigNode {
  uint32_t offset;      // Offset on the heap to the key-value entry
  uint16_t keyLength;   // Length of the key on the heap
  uint16_t valueLength; // Length of the value on the heap
  uint32_t keyHead;     // First 4 bytes of the key
} __attribute__((packed));

struct FenceKeySlotInterpolationSequentialSearchBigNode {
  uint32_t offset; // Offset of the key on the heap
  uint16_t len;    // Length of the key on the heap
};

struct BTreeNodeHeaderInterpolationSequentialSearchBigNode {
  union {
    BTreeNodeInterpolationSequentialSearchBigNode *rightMostChildInterpolationSequentialSearchBigNode;   // Pointer to the rightmost child if it's a inner node
    BTreeLeafNodeInterpolationSequentialSearchBigNode *nextLeafNodeInterpolationSequentialSearchBigNode; // Pointer to the next leaf node if it's a leaf node
  };
  bool isLeaf;
  uint32_t numKeys = 0;
  uint32_t spaceUsed = 0;
  uint32_t freeOffset = NODE_SIZE_INTERPOLATIONSEQUENTIALSEARCHBIGNODE - sizeof(BTreeNodeHeaderInterpolationSequentialSearchBigNode);
  uint16_t prefixLen = 0;
  uint32_t prefixOffset = 0;
  FenceKeySlotInterpolationSequentialSearchBigNode lowerFence = {0, 0};
  FenceKeySlotInterpolationSequentialSearchBigNode upperFence = {0, 0};
  std::array<uint32_t, 16> hints = {0};
};

class alignas(NODE_SIZE_INTERPOLATIONSEQUENTIALSEARCHBIGNODE) BTreeNodeInterpolationSequentialSearchBigNode : public BTreeNodeHeaderInterpolationSequentialSearchBigNode {
public:
  friend class BTreeInterpolationSequentialSearchBigNode;
  friend class Tester;
  static constexpr uint32_t CONTENT_SIZE = NODE_SIZE_INTERPOLATIONSEQUENTIALSEARCHBIGNODE - sizeof(BTreeNodeHeaderInterpolationSequentialSearchBigNode);
  uint8_t content[CONTENT_SIZE];

  BTreeNodeInterpolationSequentialSearchBigNode() = default;

  BTreeNodeInterpolationSequentialSearchBigNode(std::span<uint8_t> lowerFenceKey, std::span<uint8_t> upperFenceKey);

  // Returns the key without the prefix at a given positon
  std::span<uint8_t> getShortenedKey(uint32_t position) {
    auto slot = reinterpret_cast<PageSlotInterpolationSequentialSearchBigNode *>(content + position * sizeof(PageSlotInterpolationSequentialSearchBigNode));
    return std::span<uint8_t>(content + slot->offset, slot->keyLength);
  }

  // Returns the full key at a given position
  std::vector<uint8_t> getFullKey(uint32_t position) {
    auto slot = reinterpret_cast<PageSlotInterpolationSequentialSearchBigNode *>(content + position * sizeof(PageSlotInterpolationSequentialSearchBigNode));
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
  uint32_t getEntryIndexByKey(std::span<uint8_t> key);
  uint32_t sequentialSearch(std::span<uint8_t> keyWithoutPrefix, uint32_t keyHead, uint32_t startingIndex, uint32_t endIndex);
  uint32_t sequentialSearchBackwards(std::span<uint8_t> keyWithoutPrefix, uint32_t keyHead, uint32_t startingIndex, uint32_t endIndex);

  // Inserts the given key and value into the subtree of this node
  // If the return value is not empty, it contains the new child and the splitkey which needs to be inserted in the caller
  std::optional<std::pair<BTreeNodeInterpolationSequentialSearchBigNode *, std::vector<uint8_t>>> insert(std::span<uint8_t> key, std::span<uint8_t> value);

  // Removes the given key from the subtree of this node, returns true if the key was found and removed
  bool remove(std::span<uint8_t> key);

  // Destroys the subtree of this node
  void destroy();

  // Returns the index where the node should be split such that both the resulting nodes have an equal amount of free space
  // The splitIndex should remain in the current node
  uint32_t getSplitIndex();

  // Reorders the entries on the heap such that new elements can be inserted
  void compact();

  // Inserts the given key and value at the given position
  void insertEntry(uint32_t position, std::span<uint8_t> key, std::span<uint8_t> value);

  // Inserts the given key and childPointer at the given position
  void insertEntry(uint32_t position, std::span<uint8_t> key, BTreeNodeInterpolationSequentialSearchBigNode *childPointer);

  // Erases the entry at the given position
  void eraseEntry(uint32_t position);

  // Checks if the given key is smaller or equal that the key at the given position, the given key should not contain the prefix
  bool keySmallerEqualThanAtPosition(uint32_t position, uint32_t keyHead, std::span<uint8_t> key);

  // Checks if the given key is smaller or equal that the key at the given position, the given key should not contain the prefix
  bool keySmallerThanAtPosition(uint32_t position, uint32_t keyHead, std::span<uint8_t> key);

  // Checks if the given key is greater that the key at the given position, the given key should not contain the prefix
  bool keyLargerThanAtPosition(uint32_t position, uint32_t keyHead, std::span<uint8_t> key) { return !keySmallerEqualThanAtPosition(position, keyHead, key); }

  // Splits the node at the given position and returns the new node
  BTreeNodeInterpolationSequentialSearchBigNode *splitNode(uint32_t splitIndex, std::span<uint8_t> splitKey);

  std::vector<uint8_t> getFenceKey(FenceKeySlotInterpolationSequentialSearchBigNode &fenceKey) {
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

class BTreeInnerNodeInterpolationSequentialSearchBigNode : public BTreeNodeInterpolationSequentialSearchBigNode {
public:
  BTreeInnerNodeInterpolationSequentialSearchBigNode(std::span<uint8_t> lowerFenceKey, std::span<uint8_t> upperFenceKey) : BTreeNodeInterpolationSequentialSearchBigNode(lowerFenceKey, upperFenceKey) { isLeaf = false; }

  BTreeInnerNodeInterpolationSequentialSearchBigNode() { isLeaf = false; }

  // Returns the child pointer at the given position
  BTreeNodeInterpolationSequentialSearchBigNode *getChild(uint32_t position) {
    // the rightmost child is accessed
    if (position == numKeys) {
      return rightMostChildInterpolationSequentialSearchBigNode;
    }
    auto slot = reinterpret_cast<PageSlotInterpolationSequentialSearchBigNode *>(content + position * sizeof(PageSlotInterpolationSequentialSearchBigNode));
    BTreeNodeInterpolationSequentialSearchBigNode *pointer = nullptr;
    std::memcpy(&pointer, content + slot->offset + slot->keyLength, sizeof(BTreeNodeInterpolationSequentialSearchBigNode *));
    return pointer;
  }

  // Overwrites the child pointer with a new pointer at a given position, the key won't be changed
  void overwriteChild(uint32_t position, BTreeNodeInterpolationSequentialSearchBigNode *newChild);
};

class BTreeLeafNodeInterpolationSequentialSearchBigNode : public BTreeNodeInterpolationSequentialSearchBigNode {
public:
  BTreeLeafNodeInterpolationSequentialSearchBigNode(std::span<uint8_t> lowerFenceKey, std::span<uint8_t> upperFenceKey) : BTreeNodeInterpolationSequentialSearchBigNode(lowerFenceKey, upperFenceKey) { isLeaf = true; }

  BTreeLeafNodeInterpolationSequentialSearchBigNode() {
    isLeaf = true;
    nextLeafNodeInterpolationSequentialSearchBigNode = nullptr;
  }

  // Returns the value at the given position
  std::span<uint8_t> getValue(uint32_t position) {
    auto slot = reinterpret_cast<PageSlotInterpolationSequentialSearchBigNode *>(content + position * sizeof(PageSlotInterpolationSequentialSearchBigNode));
    return std::span<uint8_t>(content + slot->offset + slot->keyLength, slot->valueLength);
  }
};

#endif // ADAPTIVE_INTERPOLATION_SEARCH_IN_B_TREES_BTREE_INTERPOLATIONSEQUENTIALSEARCHBIGNODE_BTREENODE_INTERPOLATIONSEQUENTIALSEARCHBIGNODE_HPP