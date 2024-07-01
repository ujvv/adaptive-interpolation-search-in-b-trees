#include "btreenode_interpolationSequentialSearchBigNode.hpp"
#include "utils.hpp"

#include <iostream>
#include <cmath>
#include <vector>

BTreeNodeInterpolationSequentialSearchBigNode::BTreeNodeInterpolationSequentialSearchBigNode(std::span<uint8_t> lowerFenceKey, std::span<uint8_t> upperFenceKey) {
  uint16_t minLength = std::min(lowerFenceKey.size(), upperFenceKey.size());
  prefixLen = minLength;
  for (uint16_t i = 0; i < minLength; i++) {
    if (lowerFenceKey[i] != upperFenceKey[i]) {
      prefixLen = i;
      break;
    }
  }

  // Insert prefix
  if (prefixLen > 0) {
    freeOffset -= prefixLen;
    prefixOffset = freeOffset;
    std::memcpy(content + freeOffset, lowerFenceKey.data(), prefixLen);
    spaceUsed += prefixLen;
  }

  // Insert lowerFenceKey
  uint16_t lowerFenceKeySize = lowerFenceKey.size() - prefixLen;
  if (lowerFenceKeySize > 0) {
    freeOffset -= lowerFenceKeySize;
    lowerFence.len = lowerFenceKeySize;
    lowerFence.offset = freeOffset;
    std::memcpy(content + freeOffset, lowerFenceKey.data() + prefixLen, lowerFenceKeySize);
    spaceUsed += lowerFenceKeySize;
  } else {
    lowerFence.len = 0;
    lowerFence.offset = 0;
  }

  // Insert upperFenceKey
  uint16_t upperFenceKeySize = upperFenceKey.size() - prefixLen;
  if (upperFenceKeySize > 0) {
    freeOffset -= upperFenceKeySize;
    upperFence.len = upperFenceKeySize;
    upperFence.offset = freeOffset;
    std::memcpy(content + freeOffset, upperFenceKey.data() + prefixLen, upperFenceKeySize);
    spaceUsed += upperFenceKeySize;
  } else {
    upperFence.len = 0;
    upperFence.offset = 0;
  }
}

uint32_t BTreeNodeInterpolationSequentialSearchBigNode::getSplitIndex() {
  uint32_t splitIndex = 0;
  uint32_t spaceUsedSoFar = 0;
  for (uint32_t i = 0; i < numKeys; i++) {
    auto slot = reinterpret_cast<PageSlotInterpolationSequentialSearchBigNode *>(content + i * sizeof(PageSlotInterpolationSequentialSearchBigNode));
    uint32_t requiredSpace = slot->keyLength + slot->valueLength + sizeof(PageSlotInterpolationSequentialSearchBigNode);
    spaceUsedSoFar += requiredSpace;
    if (spaceUsedSoFar <= CONTENT_SIZE / 2) {
      splitIndex = i;
    } else {
      break;
    }
  }
  return std::max((uint32_t) 1, splitIndex);
}

std::vector<std::vector<uint8_t>> BTreeNodeInterpolationSequentialSearchBigNode::getKeys() {
  std::vector<std::vector<uint8_t>> keys(numKeys);
  for (uint32_t i = 0; i < numKeys; i++) {
    std::vector<uint8_t> key = getFullKey(i);
    keys[i] = key;
  }
  return keys;
}

std::vector<std::string> BTreeNodeInterpolationSequentialSearchBigNode::getKeysAsString() {
  std::vector<std::string> keys(numKeys);
  for (uint32_t i = 0; i < numKeys; i++) {
    std::vector<uint8_t> key = getFullKey(i);
    keys[i] = std::string(key.begin(), key.end());
  }
  return keys;
}

std::vector<std::string> BTreeNodeInterpolationSequentialSearchBigNode::getShortenedKeysAsString() {
  std::vector<std::string> keys(numKeys);
  for (uint32_t i = 0; i < numKeys; i++) {
    auto key = getShortenedKey(i);
    keys[i] = std::string(key.begin(), key.end());
  }
  return keys;
}

bool BTreeNodeInterpolationSequentialSearchBigNode::keySmallerEqualThanAtPosition(uint32_t position, uint32_t keyHead, std::span<uint8_t> key) {
  auto slot = reinterpret_cast<PageSlotInterpolationSequentialSearchBigNode *>(content + position * sizeof(PageSlotInterpolationSequentialSearchBigNode));
  if (keyHead < slot->keyHead) {
    return true;
  }
  if (keyHead > slot->keyHead) {
    return false;
  }

  // The key heads are equal, so we need to compare the full keys
  return key <= getShortenedKey(position);
}

bool BTreeNodeInterpolationSequentialSearchBigNode::keySmallerThanAtPosition(uint32_t position, uint32_t keyHead, std::span<uint8_t> key) {
  auto slot = reinterpret_cast<PageSlotInterpolationSequentialSearchBigNode *>(content + position * sizeof(PageSlotInterpolationSequentialSearchBigNode));
  if (keyHead < slot->keyHead) {
    return true;
  }
  if (keyHead > slot->keyHead) {
    return false;
  }

  // The key heads are equal, so we need to compare the full keys
  return key < getShortenedKey(position);
}

uint32_t BTreeNodeInterpolationSequentialSearchBigNode::getEntryIndexByKey(std::span<uint8_t> key) {
  // Do a interpolation search to find the index of the entry where the key is / should contained
  // This function assumes that the key shares the same prefix as all the keys in the node

  std::span<uint8_t> keyWithoutPrefix = key.subspan(prefixLen);
  uint32_t keyHead = getKeyHead(keyWithoutPrefix);

  if (numKeys == 0 || keySmallerEqualThanAtPosition(0, keyHead, keyWithoutPrefix)) {
    return 0;
  }
  if (numKeys > 0 && keyLargerThanAtPosition(numKeys - 1, keyHead, keyWithoutPrefix)) {
    return numKeys;
  }

  uint32_t left = 0;
  uint32_t right = numKeys - 1;
  uint32_t leftKey = getKeyHead(getShortenedKey(left));
  uint32_t dividendDecimal = keyHead - leftKey;
  uint64_t slope = (static_cast<uint64_t>(numKeys - 1) << 32) / (getKeyHead(getShortenedKey(right)) - getKeyHead(getShortenedKey(left)));
  uint32_t expected = (dividendDecimal * slope) >> 32;

  if (keySmallerEqualThanAtPosition(expected, keyHead, keyWithoutPrefix)) {
    return sequentialSearchBackwards(keyWithoutPrefix, keyHead, expected, left);
  } else {
    return sequentialSearch(keyWithoutPrefix, keyHead, expected, right);
  }
}

uint32_t BTreeNodeInterpolationSequentialSearchBigNode::sequentialSearch(std::span<uint8_t> keyWithoutPrefix, uint32_t keyHead, uint32_t startingIndex, uint32_t endIndex) {
  for (uint32_t i = startingIndex; i < endIndex; i++) {
    if (keySmallerEqualThanAtPosition(i, keyHead, keyWithoutPrefix)) {
      return i;
    }
  }
  return endIndex;
}

uint32_t BTreeNodeInterpolationSequentialSearchBigNode::sequentialSearchBackwards(std::span<uint8_t> keyWithoutPrefix, uint32_t keyHead, uint32_t startingIndex, uint32_t endIndex) {
  for (uint32_t i = startingIndex; i >= endIndex; i--) {
    if (keyLargerThanAtPosition(i, keyHead, keyWithoutPrefix)) {
      return i+1;
    }
  }
  return endIndex;
}

void BTreeNodeInterpolationSequentialSearchBigNode::compact() {
  // Only the heap needs to be restructured, so compute where the heap starts
  const uint32_t heapStart = numKeys * sizeof(PageSlotInterpolationSequentialSearchBigNode);
  const uint32_t compactableSize = CONTENT_SIZE - heapStart;
  uint8_t *buffer = reinterpret_cast<uint8_t *>(alloca(compactableSize));
  uint32_t insertionOffset = compactableSize;

  // Copy Prefix
  insertionOffset -= prefixLen;
  if (prefixLen > 0) {
    std::memcpy(buffer + insertionOffset, content + prefixOffset, prefixLen);
  }
  prefixOffset = heapStart + insertionOffset;

  // Copy the fence keys
  insertionOffset -= lowerFence.len;
  if (lowerFence.len > 0) {
    std::memcpy(buffer + insertionOffset, content + lowerFence.offset, lowerFence.len);
  }
  lowerFence.offset = heapStart + insertionOffset;

  insertionOffset -= upperFence.len;
  if (upperFence.len > 0) {
    std::memcpy(buffer + insertionOffset, content + upperFence.offset, upperFence.len);
  }
  upperFence.offset = heapStart + insertionOffset;

  // Copy all entries to the buffer, but not the slots
  for (uint32_t i = 0; i < numKeys; i++) {
    auto slot = reinterpret_cast<PageSlotInterpolationSequentialSearchBigNode *>(content + i * sizeof(PageSlotInterpolationSequentialSearchBigNode));
    uint32_t requiredSize = slot->keyLength + slot->valueLength;
    insertionOffset -= requiredSize;
    std::memcpy(buffer + insertionOffset, content + slot->offset, requiredSize);
    slot->offset = heapStart + insertionOffset;
  }

  // Copy the buffer back to the content, after the slots
  std::memcpy(content + heapStart, buffer, compactableSize);
  freeOffset = heapStart + insertionOffset;
}

void BTreeNodeInterpolationSequentialSearchBigNode::insertEntry(uint32_t position, std::span<uint8_t> key, std::span<uint8_t> value) {
  // First trim the key to get rid of the prefix
  std::span<uint8_t> keyWithoutPrefix = key.subspan(prefixLen);
  uint32_t requiredSpace = sizeof(PageSlotInterpolationSequentialSearchBigNode) + keyWithoutPrefix.size() + value.size();

  // Compact the node if there is enough space in the node, but not enough contiguous space in the heap
  bool overflows = freeOffset < requiredSpace || freeOffset - requiredSpace < static_cast<uint32_t>(numKeys * sizeof(PageSlotInterpolationSequentialSearchBigNode));
  if (overflows) {
    compact();
  }

  // Move all slots after the insertion position one slot to the right
  uint32_t moveFromOffset = position * sizeof(PageSlotInterpolationSequentialSearchBigNode);
  uint32_t moveToOffset = (position + 1) * sizeof(PageSlotInterpolationSequentialSearchBigNode);
  uint32_t moveSize = (numKeys - position) * sizeof(PageSlotInterpolationSequentialSearchBigNode);
  if (moveSize > 0) {
    std::memmove(content + moveToOffset, content + moveFromOffset, moveSize);
  }

  // Copy the key and value
  freeOffset -= (keyWithoutPrefix.size() + value.size());
  if (keyWithoutPrefix.size() > 0) {
    std::memcpy(content + freeOffset, keyWithoutPrefix.data(), keyWithoutPrefix.size());
  }
  if (value.size() > 0) {
    std::memcpy(content + freeOffset + keyWithoutPrefix.size(), value.data(), value.size());
  }

  // Store the size and offset information in the new slot
  auto slot = reinterpret_cast<PageSlotInterpolationSequentialSearchBigNode *>(content + position * sizeof(PageSlotInterpolationSequentialSearchBigNode));
  slot->offset = freeOffset;
  slot->keyLength = keyWithoutPrefix.size();
  slot->valueLength = value.size();
  slot->keyHead = getKeyHead(keyWithoutPrefix);

  // Update the node state
  numKeys++;
  spaceUsed += requiredSpace;
}
void BTreeNodeInterpolationSequentialSearchBigNode::insertEntry(uint32_t position, std::span<uint8_t> key, BTreeNodeInterpolationSequentialSearchBigNode *childPointer) { insertEntry(position, key, std::span<uint8_t>(reinterpret_cast<uint8_t *>(&childPointer), sizeof(BTreeNodeInterpolationSequentialSearchBigNode *))); }

void BTreeNodeInterpolationSequentialSearchBigNode::eraseEntry(uint32_t position) {
  // If the rightmost child gets deleted, set the rightmost child to the child before it
  if (position == numKeys && !isLeaf) {
    rightMostChildInterpolationSequentialSearchBigNode = reinterpret_cast<BTreeInnerNodeInterpolationSequentialSearchBigNode *>(this)->getChild(position - 1);
    eraseEntry(position - 1);
    return;
  }

  // Update the node state
  auto slot = reinterpret_cast<PageSlotInterpolationSequentialSearchBigNode *>(content + position * sizeof(PageSlotInterpolationSequentialSearchBigNode));
  spaceUsed -= (sizeof(PageSlotInterpolationSequentialSearchBigNode) + slot->keyLength + slot->valueLength);

  // Move the slots after the deleted slot one slot to the left
  uint32_t moveFromOffset = (position + 1) * sizeof(PageSlotInterpolationSequentialSearchBigNode);
  uint32_t moveToOffset = position * sizeof(PageSlotInterpolationSequentialSearchBigNode);
  uint32_t moveSize = (numKeys - position - 1) * sizeof(PageSlotInterpolationSequentialSearchBigNode);
  std::memmove(content + moveToOffset, content + moveFromOffset, moveSize);

  numKeys--;
}

void BTreeInnerNodeInterpolationSequentialSearchBigNode::overwriteChild(uint32_t position, BTreeNodeInterpolationSequentialSearchBigNode *newChild) {
  auto slot = reinterpret_cast<PageSlotInterpolationSequentialSearchBigNode *>(content + position * sizeof(PageSlotInterpolationSequentialSearchBigNode));
  std::memcpy(content + slot->offset + slot->keyLength, &newChild, sizeof(BTreeNodeInterpolationSequentialSearchBigNode *));
}

void BTreeNodeInterpolationSequentialSearchBigNode::destroy() {
  if (isLeaf) {
    delete reinterpret_cast<BTreeLeafNodeInterpolationSequentialSearchBigNode *>(this);
  } else {
    BTreeInnerNodeInterpolationSequentialSearchBigNode *innerNode = reinterpret_cast<BTreeInnerNodeInterpolationSequentialSearchBigNode *>(this);
    for (uint32_t i = 0; i <= numKeys; ++i) {
      innerNode->getChild(i)->destroy();
    }
    delete innerNode;
  }
}

BTreeNodeInterpolationSequentialSearchBigNode *BTreeNodeInterpolationSequentialSearchBigNode::splitNode(uint32_t splitIndex, std::span<uint8_t> splitKey) {
  // Create the new node
  BTreeNodeInterpolationSequentialSearchBigNode *newNode = nullptr;
  auto upperFenceKey = getUpperFenceKey();
  if (isLeaf) {
    newNode = new BTreeLeafNodeInterpolationSequentialSearchBigNode(splitKey, upperFenceKey);
  } else {
    newNode = new BTreeInnerNodeInterpolationSequentialSearchBigNode(splitKey, upperFenceKey);
  }

  // Copy the entries starting from splitIndex+1 to the new node
  uint32_t initialEntryCount = numKeys;
  uint32_t insertIndex = 0;

  for (uint32_t i = splitIndex; i < initialEntryCount; ++i) {
    auto key = getFullKey(i);
    if (isLeaf) {
      auto value = reinterpret_cast<BTreeLeafNodeInterpolationSequentialSearchBigNode *>(this)->getValue(i);
      newNode->insertEntry(insertIndex, key, value);
    } else {
      auto child = reinterpret_cast<BTreeInnerNodeInterpolationSequentialSearchBigNode *>(this)->getChild(i);
      newNode->insertEntry(insertIndex, key, child);
    }
    ++insertIndex;
  }

  // If it's a leaf, update the linkedlist
  if (isLeaf) {
    newNode->nextLeafNodeInterpolationSequentialSearchBigNode = nextLeafNodeInterpolationSequentialSearchBigNode;
    nextLeafNodeInterpolationSequentialSearchBigNode = reinterpret_cast<BTreeLeafNodeInterpolationSequentialSearchBigNode *>(newNode);
  }

  return newNode;
}

std::optional<std::pair<BTreeNodeInterpolationSequentialSearchBigNode *, std::vector<uint8_t>>> BTreeNodeInterpolationSequentialSearchBigNode::insert(std::span<uint8_t> key, std::span<uint8_t> value) {
  if (!isLeaf) {
    uint32_t childIndex = getEntryIndexByKey(key);
    auto toInsert = reinterpret_cast<BTreeInnerNodeInterpolationSequentialSearchBigNode *>(this)->getChild(childIndex)->insert(key, value);
    if (!toInsert.has_value()) {
      return std::nullopt;
    }

    // Insert the new node
    BTreeNodeInterpolationSequentialSearchBigNode *nodeToInsert = toInsert->first;
    std::vector<uint8_t> keyToInsert = toInsert->second;
    uint32_t requiredSpace = sizeof(PageSlotInterpolationSequentialSearchBigNode) + keyToInsert.size() + sizeof(BTreeNodeInterpolationSequentialSearchBigNode *);
    bool canFit = spaceUsed + requiredSpace <= CONTENT_SIZE;
    BTreeNodeInterpolationSequentialSearchBigNode* currentChild = reinterpret_cast<BTreeInnerNodeInterpolationSequentialSearchBigNode *>(this)->getChild(childIndex);

    // If the entry can fit, insert it and then the insertion process is finished
    if (canFit) {
      if (childIndex == numKeys) {
        insertEntry(childIndex, keyToInsert, currentChild);
        reinterpret_cast<BTreeInnerNodeInterpolationSequentialSearchBigNode *>(this)->rightMostChildInterpolationSequentialSearchBigNode = nodeToInsert;
      } else {
        reinterpret_cast<BTreeInnerNodeInterpolationSequentialSearchBigNode *>(this)->overwriteChild(childIndex, nodeToInsert);
        insertEntry(childIndex, keyToInsert, currentChild);
      }
      return std::nullopt;
    }

    // Split the node
    uint32_t splitIndex = getSplitIndex();
    uint32_t insertIndex = childIndex;
    if (splitIndex == insertIndex) {
      splitIndex--;
    }
    std::vector<uint8_t> splitKey = getFullKey(splitIndex);

    // Create the new right sibling, the new node with the entries [splitIndex+1, numKeys]
    BTreeInnerNodeInterpolationSequentialSearchBigNode *newRightSibling = reinterpret_cast<BTreeInnerNodeInterpolationSequentialSearchBigNode *>(splitNode(splitIndex + 1, splitKey));
    newRightSibling->rightMostChildInterpolationSequentialSearchBigNode = reinterpret_cast<BTreeInnerNodeInterpolationSequentialSearchBigNode *>(this)->rightMostChildInterpolationSequentialSearchBigNode;

    // Create a new node that will replace the current node because the prefixLen could change so all keys need to be reinserted
    auto lowerFenceKey = getLowerFenceKey();
    BTreeInnerNodeInterpolationSequentialSearchBigNode *newThisChild = new BTreeInnerNodeInterpolationSequentialSearchBigNode(lowerFenceKey, splitKey);

    // Insert all the entreis remaining in the current node into the new node
    for (uint32_t i = 0; i <= splitIndex; ++i) {
      auto key = getFullKey(i);
      auto child = reinterpret_cast<BTreeInnerNodeInterpolationSequentialSearchBigNode *>(this)->getChild(i);
      newThisChild->insertEntry(i, key, child);
    }

    // Insert the new entry
    if (insertIndex < splitIndex) {
      newThisChild->overwriteChild(insertIndex, nodeToInsert);
      newThisChild->insertEntry(insertIndex, keyToInsert, currentChild);
    }
    newThisChild->eraseEntry(newThisChild->numKeys);

    if (insertIndex > splitIndex) {
      insertIndex = insertIndex - splitIndex - 1;
      // The new child will be the rightmost
      if (insertIndex == newRightSibling->numKeys) {
        newRightSibling->insertEntry(insertIndex, keyToInsert, currentChild);
        newRightSibling->rightMostChildInterpolationSequentialSearchBigNode = nodeToInsert;
      } else {
        newRightSibling->overwriteChild(insertIndex, nodeToInsert);
        newRightSibling->insertEntry(insertIndex, keyToInsert, currentChild);
      }
    }

    // Swap the new node with the current one, delete the current one and return the new node to insert into the parent
    // with the corresponding split key
    std::swap(*reinterpret_cast<BTreeInnerNodeInterpolationSequentialSearchBigNode *>(this), *newThisChild);
    delete newThisChild;
    return std::make_pair(newRightSibling, splitKey);

  } else {
    std::span<uint8_t> keyWithoutPrefix = key.subspan(prefixLen);
    uint32_t insertIndex = getEntryIndexByKey(key);

    // Handle the case when the key already exists
    if (insertIndex < numKeys && keyWithoutPrefix == getShortenedKey(insertIndex)) {
      eraseEntry(insertIndex);
    }

    uint32_t requiredSpace = sizeof(PageSlotInterpolationSequentialSearchBigNode) + keyWithoutPrefix.size() + value.size();
    bool canFit = spaceUsed + requiredSpace <= CONTENT_SIZE;

    // If the key + value can fit in the current node, insert the entry
    if (canFit) {
      insertEntry(insertIndex, key, value);
      return std::nullopt;
    }

    // Otherwise we need to split the node
    uint32_t splitIndex = getSplitIndex();
    std::vector<uint8_t> splitKey;
    BTreeLeafNodeInterpolationSequentialSearchBigNode *newRightSibling = nullptr;
    uint32_t lastEntryIndex;

    // Determine the correct splitKey and the new right sibling as well as the index of the last key that should remain
    // In the current node
    if (insertIndex < splitIndex) {
      splitKey = getFullKey(splitIndex - 1);
      newRightSibling = reinterpret_cast<BTreeLeafNodeInterpolationSequentialSearchBigNode *>(splitNode(splitIndex, splitKey));
      lastEntryIndex = splitIndex - 1;
    } else if (insertIndex > splitIndex) {
      splitKey = getFullKey(splitIndex);
      newRightSibling = reinterpret_cast<BTreeLeafNodeInterpolationSequentialSearchBigNode *>(splitNode(splitIndex + 1, splitKey));
      lastEntryIndex = splitIndex;
    } else {
      splitKey = std::vector<uint8_t>(key.begin(), key.end());
      newRightSibling = reinterpret_cast<BTreeLeafNodeInterpolationSequentialSearchBigNode *>(splitNode(splitIndex, splitKey));
      lastEntryIndex = splitIndex - 1;
    }

    // Create a new node that will replace the current node because the prefixLen could change so all keys need to be reinserted
    auto lowerFenceKey = getLowerFenceKey();
    BTreeLeafNodeInterpolationSequentialSearchBigNode *newThisChild = new BTreeLeafNodeInterpolationSequentialSearchBigNode(lowerFenceKey, splitKey);
    newThisChild->nextLeafNodeInterpolationSequentialSearchBigNode = newRightSibling;

    // Insert all the entreis remaining in the current node into the new node
    for (uint32_t i = 0; i <= lastEntryIndex; ++i) {
      auto key = getFullKey(i);
      auto value = reinterpret_cast<BTreeLeafNodeInterpolationSequentialSearchBigNode *>(this)->getValue(i);
      newThisChild->insertEntry(i, key, value);
    }

    // Insert the new entry
    if (insertIndex <= splitIndex) {
      newThisChild->insertEntry(insertIndex, key, value);
    } else {
      insertIndex = insertIndex - splitIndex - 1;
      newRightSibling->insertEntry(insertIndex, key, value);
    }

    // Swap the new node with the current one, delete the current one and return the new node to insert into the parent
    std::swap(*reinterpret_cast<BTreeLeafNodeInterpolationSequentialSearchBigNode *>(this), *newThisChild);
    delete newThisChild;
    return std::make_pair(newRightSibling, splitKey);
  }
}

bool BTreeNodeInterpolationSequentialSearchBigNode::remove(std::span<uint8_t> key) {
  if (isLeaf) {
    uint32_t entryIndex = getEntryIndexByKey(key);
    if (entryIndex < numKeys) {
      auto existingKey = getFullKey(entryIndex);
      bool keysEqual = key.size() == existingKey.size() && std::equal(key.begin(), key.end(), existingKey.begin());
      if (keysEqual) {
        eraseEntry(entryIndex);
        return true;
      }
    }
    return false;
  } else {
    uint32_t childIndex = getEntryIndexByKey(key);
    return reinterpret_cast<BTreeInnerNodeInterpolationSequentialSearchBigNode *>(this)->getChild(childIndex)->remove(key);
  }
}
