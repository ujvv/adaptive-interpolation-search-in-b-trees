#ifndef ADAPTIVE_INTERPOLATION_SEARCH_IN_B_TREES_BTREE_TEMPLATE_BTREENODE_TEMPLATE_HPP
#define ADAPTIVE_INTERPOLATION_SEARCH_IN_B_TREES_BTREE_TEMPLATE_BTREENODE_TEMPLATE_HPP

#include <cstdint>
#include <cstring>
#include <functional>
#include <optional>
#include <span>
#include <string>


constexpr uint32_t NODE_SIZE_TEMPLATE = 4096;
// constexpr uint32_t NODE_SIZE_TEMPLATE = 8192;


// Forward declarations
class BTreeNodeTemplate;
class BTreeLeafNodeTemplate;

struct PageSlotTemplate {
  uint16_t offset;      // Offset on the heap to the key-value entry
  uint16_t keyLength;   // Length of the key on the heap
  uint16_t valueLength; // Length of the value on the heap
  uint32_t keyHead;     // First 4 bytes of the key
} __attribute__((packed));

struct FenceKeySlotTemplate {
  uint16_t offset; // Offset of the key on the heap
  uint16_t len;    // Length of the key on the heap
};

struct BTreeNodeHeaderTemplate {
  union {
    BTreeNodeTemplate *rightMostChildTemplate;   // Pointer to the rightmost child if it's a inner node
    BTreeLeafNodeTemplate *nextLeafNodeTemplate; // Pointer to the next leaf node if it's a leaf node
  };
  bool isLeaf;
  uint16_t numKeys = 0;
  uint16_t spaceUsed = 0;
  uint16_t freeOffset = NODE_SIZE_TEMPLATE - sizeof(BTreeNodeHeaderTemplate);
  uint16_t prefixLen = 0;
  uint16_t prefixOffset = 0;
  FenceKeySlotTemplate lowerFence = {0, 0};
  FenceKeySlotTemplate upperFence = {0, 0};
  std::array<uint32_t, 16> hints = {0};
};

class alignas(NODE_SIZE_TEMPLATE) BTreeNodeTemplate : public BTreeNodeHeaderTemplate {
public:
  friend class BTreeTemplate;
  friend class Tester;
  static constexpr uint32_t CONTENT_SIZE = NODE_SIZE_TEMPLATE - sizeof(BTreeNodeHeaderTemplate);
  uint8_t content[CONTENT_SIZE];

  BTreeNodeTemplate() = default;

  BTreeNodeTemplate(std::span<uint8_t> lowerFenceKey, std::span<uint8_t> upperFenceKey);

  // Returns the key without the prefix at a given positon
  std::span<uint8_t> getShortenedKey(uint16_t position) {
    auto slot = reinterpret_cast<PageSlotTemplate *>(content + position * sizeof(PageSlotTemplate));
    return std::span<uint8_t>(content + slot->offset, slot->keyLength);
  }

  // Returns the full key at a given position
  std::vector<uint8_t> getFullKey(uint16_t position) {
    auto slot = reinterpret_cast<PageSlotTemplate *>(content + position * sizeof(PageSlotTemplate));
    std::vector<uint8_t> key(slot->keyLength + prefixLen);

    if (prefixLen > 0) {
      std::memcpy(key.data(), content + lowerFence.offset, prefixLen);
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
  std::optional<std::pair<BTreeNodeTemplate *, std::vector<uint8_t>>> insert(std::span<uint8_t> key, std::span<uint8_t> value);

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
  void insertEntry(uint16_t position, std::span<uint8_t> key, BTreeNodeTemplate *childPointer);

  // Erases the entry at the given position
  void eraseEntry(uint16_t position);

  // Checks if the given key is smaller or equal that the key at the given position, the given key should not contain the prefix
  bool keySmallerEqualThanAtPosition(uint16_t position, uint32_t keyHead, std::span<uint8_t> key);

  // Checks if the given key is greater that the key at the given position, the given key should not contain the prefix
  bool keyLargerThanAtPosition(uint16_t position, uint32_t keyHead, std::span<uint8_t> key) { return !keySmallerEqualThanAtPosition(position, keyHead, key); }

  // Recalculates the hints array
  void updateHints();

  // Splits the node at the given position and returns the new node
  BTreeNodeTemplate *splitNode(uint16_t splitIndex, std::span<uint8_t> splitKey);

  // Returns the range of possible positions where the key with the given keyHead is located
  std::pair<int, int> getHintRange(uint32_t keyHead);

  std::vector<uint8_t> getFenceKey(FenceKeySlotTemplate &fenceKey) {
    if (prefixLen == 0 || fenceKey.len == 0) {
      return {};
    }
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

class BTreeInnerNodeTemplate : public BTreeNodeTemplate {
public:
  BTreeInnerNodeTemplate(std::span<uint8_t> lowerFenceKey, std::span<uint8_t> upperFenceKey) : BTreeNodeTemplate(lowerFenceKey, upperFenceKey) { isLeaf = false; }

  BTreeInnerNodeTemplate() { isLeaf = false; }

  // Returns the child pointer at the given position
  BTreeNodeTemplate *getChild(uint16_t position) {
    // the rightmost child is accessed
    if (position == numKeys) {
      return rightMostChildTemplate;
    }
    auto slot = reinterpret_cast<PageSlotTemplate *>(content + position * sizeof(PageSlotTemplate));
    BTreeNodeTemplate *pointer = nullptr;
    std::memcpy(&pointer, content + slot->offset + slot->keyLength, sizeof(BTreeNodeTemplate *));
    return pointer;
  }

  // Overwrites the child pointer with a new pointer at a given position, the key won't be changed
  void overwriteChild(uint16_t position, BTreeNodeTemplate *newChild);
};

class BTreeLeafNodeTemplate : public BTreeNodeTemplate {
public:
  BTreeLeafNodeTemplate(std::span<uint8_t> lowerFenceKey, std::span<uint8_t> upperFenceKey) : BTreeNodeTemplate(lowerFenceKey, upperFenceKey) { isLeaf = true; }

  BTreeLeafNodeTemplate() {
    isLeaf = true;
    nextLeafNodeTemplate = nullptr;
  }

  // Returns the value at the given position
  std::span<uint8_t> getValue(uint16_t position) {
    auto slot = reinterpret_cast<PageSlotTemplate *>(content + position * sizeof(PageSlotTemplate));
    return std::span<uint8_t>(content + slot->offset + slot->keyLength, slot->valueLength);
  }
};

#endif // ADAPTIVE_INTERPOLATION_SEARCH_IN_B_TREES_BTREE_TEMPLATE_BTREENODE_TEMPLATE_HPP