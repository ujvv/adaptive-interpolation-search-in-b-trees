#include "btreenode_binarySearchHints.hpp"
#include "utils.hpp"

#include <iostream>

BTreeNodeBinarySearchHints::BTreeNodeBinarySearchHints(std::span<uint8_t> lowerFenceKey, std::span<uint8_t> upperFenceKey) {
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

uint32_t BTreeNodeBinarySearchHints::getSplitIndex() {
  uint32_t splitIndex = 0;
  uint32_t spaceUsedSoFar = 0;
  for (uint32_t i = 0; i < numKeys; i++) {
    auto slot = reinterpret_cast<PageSlotBinarySearchHints *>(content + i * sizeof(PageSlotBinarySearchHints));
    uint32_t requiredSpace = slot->keyLength + slot->valueLength + sizeof(PageSlotBinarySearchHints);
    spaceUsedSoFar += requiredSpace;
    if (spaceUsedSoFar <= CONTENT_SIZE / 2) {
      splitIndex = i;
    } else {
      break;
    }
  }
  return std::max((uint32_t) 1, splitIndex);
}

std::vector<std::vector<uint8_t>> BTreeNodeBinarySearchHints::getKeys() {
  std::vector<std::vector<uint8_t>> keys(numKeys);
  for (uint32_t i = 0; i < numKeys; i++) {
    std::vector<uint8_t> key = getFullKey(i);
    keys[i] = key;
  }
  return keys;
}

std::vector<std::string> BTreeNodeBinarySearchHints::getKeysAsString() {
  std::vector<std::string> keys(numKeys);
  for (uint32_t i = 0; i < numKeys; i++) {
    std::vector<uint8_t> key = getFullKey(i);
    keys[i] = std::string(key.begin(), key.end());
  }
  return keys;
}

std::vector<std::string> BTreeNodeBinarySearchHints::getShortenedKeysAsString() {
  std::vector<std::string> keys(numKeys);
  for (uint32_t i = 0; i < numKeys; i++) {
    auto key = getShortenedKey(i);
    keys[i] = std::string(key.begin(), key.end());
  }
  return keys;
}

bool BTreeNodeBinarySearchHints::keySmallerEqualThanAtPosition(uint32_t position, uint32_t keyHead, std::span<uint8_t> key) {
  auto slot = reinterpret_cast<PageSlotBinarySearchHints *>(content + position * sizeof(PageSlotBinarySearchHints));
  if (keyHead < slot->keyHead) {
    return true;
  }
  if (keyHead > slot->keyHead) {
    return false;
  }

  // The key heads are equal, so we need to compare the full keys
  return key <= getShortenedKey(position);
}

void BTreeNodeBinarySearchHints::updateHints() {
  if (numKeys <= 16) {
    return;
  }

  uint32_t spacing = numKeys / 16;
  for (uint32_t i = 0; i < 16; i++) {
    auto slot = reinterpret_cast<PageSlotBinarySearchHints *>(content + i * spacing * sizeof(PageSlotBinarySearchHints));
    hints[i] = slot->keyHead;
  }
}

std::pair<int, int> BTreeNodeBinarySearchHints::getHintRange(uint32_t keyHead) {
  if (numKeys <= 16) {
    return std::make_pair(0, numKeys - 1);
  }
  uint32_t spacing = numKeys / 16;
  if (keyHead < hints[0]) {
    return std::make_pair(0, spacing - 1);
  }
  if (keyHead > hints[15]) {
    return std::make_pair(15 * spacing, numKeys - 1);
  }

  // Linear search on the hints to find the left and right boundary
  uint32_t left = 0;
  while (left + 1 < 16 && keyHead > hints[left + 1]) {
    left++;
  }
  uint32_t right = 15;
  while (right - 1 >= 0 && keyHead < hints[right - 1]) {
    right--;
  }

  // If right was not moved, it should point to the last entry instead of right*spacing to account for
  // the fact that the last partition of the keys will have more keys if numKeys%16 != 0
  if (right == 15) {
    return std::make_pair(left * spacing, numKeys - 1);
  }
  return std::make_pair(left * spacing, right * spacing);
}

uint32_t BTreeNodeBinarySearchHints::getEntryIndexByKey(std::span<uint8_t> key) {
  // Do a binary search to find the index of the entry where the key is / should contained
  // This function assumes that the key shares the same prefix as all the keys in the node

  std::span<uint8_t> keyWithoutPrefix = key.subspan(prefixLen);
  uint32_t keyHead = getKeyHead(keyWithoutPrefix);

  if (numKeys == 0 || keySmallerEqualThanAtPosition(0, keyHead, keyWithoutPrefix)) {
    return 0;
  }
  if (numKeys > 0 && keyLargerThanAtPosition(numKeys - 1, keyHead, keyWithoutPrefix)) {
    return numKeys;
  }

  auto [left, right] = getHintRange(keyHead);
  uint32_t childIndex = right;

  while (left <= right) {
    uint32_t middle = (left + right) / 2;
    if (keySmallerEqualThanAtPosition(middle, keyHead, keyWithoutPrefix)) {
      childIndex = middle;
      right = middle - 1;
    } else {
      left = middle + 1;
    }
  }

  return childIndex;
}

void BTreeNodeBinarySearchHints::compact() {
  // Only the heap needs to be restructured, so compute where the heap starts
  const uint32_t heapStart = numKeys * sizeof(PageSlotBinarySearchHints);
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
    auto slot = reinterpret_cast<PageSlotBinarySearchHints *>(content + i * sizeof(PageSlotBinarySearchHints));
    uint32_t requiredSize = slot->keyLength + slot->valueLength;
    insertionOffset -= requiredSize;
    std::memcpy(buffer + insertionOffset, content + slot->offset, requiredSize);
    slot->offset = heapStart + insertionOffset;
  }

  // Copy the buffer back to the content, after the slots
  std::memcpy(content + heapStart, buffer, compactableSize);
  freeOffset = heapStart + insertionOffset;
}

void BTreeNodeBinarySearchHints::insertEntry(uint32_t position, std::span<uint8_t> key, std::span<uint8_t> value) {
  // First trim the key to get rid of the prefix
  std::span<uint8_t> keyWithoutPrefix = key.subspan(prefixLen);
  uint32_t requiredSpace = sizeof(PageSlotBinarySearchHints) + keyWithoutPrefix.size() + value.size();

  // Compact the node if there is enough space in the node, but not enough contiguous space in the heap
  bool overflows = freeOffset < requiredSpace || freeOffset - requiredSpace < static_cast<uint32_t>(numKeys * sizeof(PageSlotBinarySearchHints));
  if (overflows) {
    compact();
  }

  // Move all slots after the insertion position one slot to the right
  uint32_t moveFromOffset = position * sizeof(PageSlotBinarySearchHints);
  uint32_t moveToOffset = (position + 1) * sizeof(PageSlotBinarySearchHints);
  uint32_t moveSize = (numKeys - position) * sizeof(PageSlotBinarySearchHints);
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
  auto slot = reinterpret_cast<PageSlotBinarySearchHints *>(content + position * sizeof(PageSlotBinarySearchHints));
  slot->offset = freeOffset;
  slot->keyLength = keyWithoutPrefix.size();
  slot->valueLength = value.size();
  slot->keyHead = getKeyHead(keyWithoutPrefix);

  // Update the node state
  numKeys++;
  spaceUsed += requiredSpace;
}
void BTreeNodeBinarySearchHints::insertEntry(uint32_t position, std::span<uint8_t> key, BTreeNodeBinarySearchHints *childPointer) { insertEntry(position, key, std::span<uint8_t>(reinterpret_cast<uint8_t *>(&childPointer), sizeof(BTreeNodeBinarySearchHints *))); }

void BTreeNodeBinarySearchHints::eraseEntry(uint32_t position) {
  // If the rightmost child gets deleted, set the rightmost child to the child before it
  if (position == numKeys && !isLeaf) {
    rightMostChildBinarySearchHints = reinterpret_cast<BTreeInnerNodeBinarySearchHints *>(this)->getChild(position - 1);
    eraseEntry(position - 1);
    return;
  }

  // Update the node state
  auto slot = reinterpret_cast<PageSlotBinarySearchHints *>(content + position * sizeof(PageSlotBinarySearchHints));
  spaceUsed -= (sizeof(PageSlotBinarySearchHints) + slot->keyLength + slot->valueLength);

  // Move the slots after the deleted slot one slot to the left
  uint32_t moveFromOffset = (position + 1) * sizeof(PageSlotBinarySearchHints);
  uint32_t moveToOffset = position * sizeof(PageSlotBinarySearchHints);
  uint32_t moveSize = (numKeys - position - 1) * sizeof(PageSlotBinarySearchHints);
  std::memmove(content + moveToOffset, content + moveFromOffset, moveSize);

  numKeys--;
}

void BTreeInnerNodeBinarySearchHints::overwriteChild(uint32_t position, BTreeNodeBinarySearchHints *newChild) {
  auto slot = reinterpret_cast<PageSlotBinarySearchHints *>(content + position * sizeof(PageSlotBinarySearchHints));
  std::memcpy(content + slot->offset + slot->keyLength, &newChild, sizeof(BTreeNodeBinarySearchHints *));
}

void BTreeNodeBinarySearchHints::destroy() {
  if (isLeaf) {
    delete reinterpret_cast<BTreeLeafNodeBinarySearchHints *>(this);
  } else {
    BTreeInnerNodeBinarySearchHints *innerNode = reinterpret_cast<BTreeInnerNodeBinarySearchHints *>(this);
    for (uint32_t i = 0; i <= numKeys; ++i) {
      innerNode->getChild(i)->destroy();
    }
    delete innerNode;
  }
}

BTreeNodeBinarySearchHints *BTreeNodeBinarySearchHints::splitNode(uint32_t splitIndex, std::span<uint8_t> splitKey) {
  // Create the new node
  BTreeNodeBinarySearchHints *newNode = nullptr;
  auto upperFenceKey = getUpperFenceKey();
  if (isLeaf) {
    newNode = new BTreeLeafNodeBinarySearchHints(splitKey, upperFenceKey);
  } else {
    newNode = new BTreeInnerNodeBinarySearchHints(splitKey, upperFenceKey);
  }

  // Copy the entries starting from splitIndex+1 to the new node
  uint32_t initialEntryCount = numKeys;
  uint32_t insertIndex = 0;

  for (uint32_t i = splitIndex; i < initialEntryCount; ++i) {
    auto key = getFullKey(i);
    if (isLeaf) {
      auto value = reinterpret_cast<BTreeLeafNodeBinarySearchHints *>(this)->getValue(i);
      newNode->insertEntry(insertIndex, key, value);
    } else {
      auto child = reinterpret_cast<BTreeInnerNodeBinarySearchHints *>(this)->getChild(i);
      newNode->insertEntry(insertIndex, key, child);
    }
    ++insertIndex;
  }

  // If it's a leaf, update the linkedlist
  if (isLeaf) {
    newNode->nextLeafNodeBinarySearchHints = nextLeafNodeBinarySearchHints;
    nextLeafNodeBinarySearchHints = reinterpret_cast<BTreeLeafNodeBinarySearchHints *>(newNode);
  }

  return newNode;
}

std::optional<std::pair<BTreeNodeBinarySearchHints *, std::vector<uint8_t>>> BTreeNodeBinarySearchHints::insert(std::span<uint8_t> key, std::span<uint8_t> value) {
  if (!isLeaf) {
    uint32_t childIndex = getEntryIndexByKey(key);
    auto toInsert = reinterpret_cast<BTreeInnerNodeBinarySearchHints *>(this)->getChild(childIndex)->insert(key, value);
    if (!toInsert.has_value()) {
      return std::nullopt;
    }

    // Insert the new node
    BTreeNodeBinarySearchHints *nodeToInsert = toInsert->first;
    std::vector<uint8_t> keyToInsert = toInsert->second;
    uint32_t requiredSpace = sizeof(PageSlotBinarySearchHints) + keyToInsert.size() + sizeof(BTreeNodeBinarySearchHints *);
    bool canFit = spaceUsed + requiredSpace <= CONTENT_SIZE;
    BTreeNodeBinarySearchHints* currentChild = reinterpret_cast<BTreeInnerNodeBinarySearchHints *>(this)->getChild(childIndex);

    // If the entry can fit, insert it and then the insertion process is finished
    if (canFit) {
      if (childIndex == numKeys) {
        insertEntry(childIndex, keyToInsert, currentChild);
        reinterpret_cast<BTreeInnerNodeBinarySearchHints *>(this)->rightMostChildBinarySearchHints = nodeToInsert;
      } else {
        reinterpret_cast<BTreeInnerNodeBinarySearchHints *>(this)->overwriteChild(childIndex, nodeToInsert);
        insertEntry(childIndex, keyToInsert, currentChild);
      }
      updateHints();
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
    BTreeInnerNodeBinarySearchHints *newRightSibling = reinterpret_cast<BTreeInnerNodeBinarySearchHints *>(splitNode(splitIndex + 1, splitKey));
    newRightSibling->rightMostChildBinarySearchHints = reinterpret_cast<BTreeInnerNodeBinarySearchHints *>(this)->rightMostChildBinarySearchHints;

    // Create a new node that will replace the current node because the prefixLen could change so all keys need to be reinserted
    auto lowerFenceKey = getLowerFenceKey();
    BTreeInnerNodeBinarySearchHints *newThisChild = new BTreeInnerNodeBinarySearchHints(lowerFenceKey, splitKey);

    // Insert all the entreis remaining in the current node into the new node
    for (uint32_t i = 0; i <= splitIndex; ++i) {
      auto key = getFullKey(i);
      auto child = reinterpret_cast<BTreeInnerNodeBinarySearchHints *>(this)->getChild(i);
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
        newRightSibling->rightMostChildBinarySearchHints = nodeToInsert;
      } else {
        newRightSibling->overwriteChild(insertIndex, nodeToInsert);
        newRightSibling->insertEntry(insertIndex, keyToInsert, currentChild);
      }
    }

    newThisChild->updateHints();
    newRightSibling->updateHints();

    // Swap the new node with the current one, delete the current one and return the new node to insert into the parent
    // with the corresponding split key
    std::swap(*reinterpret_cast<BTreeInnerNodeBinarySearchHints *>(this), *newThisChild);
    delete newThisChild;
    return std::make_pair(newRightSibling, splitKey);

  } else {
    std::span<uint8_t> keyWithoutPrefix = key.subspan(prefixLen);
    uint32_t insertIndex = getEntryIndexByKey(key);

    // Handle the case when the key already exists
    if (insertIndex < numKeys && keyWithoutPrefix == getShortenedKey(insertIndex)) {
      eraseEntry(insertIndex);
    }

    uint32_t requiredSpace = sizeof(PageSlotBinarySearchHints) + keyWithoutPrefix.size() + value.size();
    bool canFit = spaceUsed + requiredSpace <= CONTENT_SIZE;

    // If the key + value can fit in the current node, insert the entry
    if (canFit) {
      insertEntry(insertIndex, key, value);
      updateHints();
      return std::nullopt;
    }

    // Otherwise we need to split the node
    uint32_t splitIndex = getSplitIndex();
    std::vector<uint8_t> splitKey;
    BTreeLeafNodeBinarySearchHints *newRightSibling = nullptr;
    uint32_t lastEntryIndex;

    // Determine the correct splitKey and the new right sibling as well as the index of the last key that should remain
    // In the current node
    if (insertIndex < splitIndex) {
      splitKey = getFullKey(splitIndex - 1);
      newRightSibling = reinterpret_cast<BTreeLeafNodeBinarySearchHints *>(splitNode(splitIndex, splitKey));
      lastEntryIndex = splitIndex - 1;
    } else if (insertIndex > splitIndex) {
      splitKey = getFullKey(splitIndex);
      newRightSibling = reinterpret_cast<BTreeLeafNodeBinarySearchHints *>(splitNode(splitIndex + 1, splitKey));
      lastEntryIndex = splitIndex;
    } else {
      splitKey = std::vector<uint8_t>(key.begin(), key.end());
      newRightSibling = reinterpret_cast<BTreeLeafNodeBinarySearchHints *>(splitNode(splitIndex, splitKey));
      lastEntryIndex = splitIndex - 1;
    }

    // Create a new node that will replace the current node because the prefixLen could change so all keys need to be reinserted
    auto lowerFenceKey = getLowerFenceKey();
    BTreeLeafNodeBinarySearchHints *newThisChild = new BTreeLeafNodeBinarySearchHints(lowerFenceKey, splitKey);
    newThisChild->nextLeafNodeBinarySearchHints = newRightSibling;

    // Insert all the entreis remaining in the current node into the new node
    for (uint32_t i = 0; i <= lastEntryIndex; ++i) {
      auto key = getFullKey(i);
      auto value = reinterpret_cast<BTreeLeafNodeBinarySearchHints *>(this)->getValue(i);
      newThisChild->insertEntry(i, key, value);
    }

    // Insert the new entry
    if (insertIndex <= splitIndex) {
      newThisChild->insertEntry(insertIndex, key, value);
    } else {
      insertIndex = insertIndex - splitIndex - 1;
      newRightSibling->insertEntry(insertIndex, key, value);
    }

    newRightSibling->updateHints();
    newThisChild->updateHints();

    // Swap the new node with the current one, delete the current one and return the new node to insert into the parent
    std::swap(*reinterpret_cast<BTreeLeafNodeBinarySearchHints *>(this), *newThisChild);
    delete newThisChild;
    return std::make_pair(newRightSibling, splitKey);
  }
}

bool BTreeNodeBinarySearchHints::remove(std::span<uint8_t> key) {
  if (isLeaf) {
    uint32_t entryIndex = getEntryIndexByKey(key);
    if (entryIndex < numKeys) {
      auto existingKey = getFullKey(entryIndex);
      bool keysEqual = key.size() == existingKey.size() && std::equal(key.begin(), key.end(), existingKey.begin());
      if (keysEqual) {
        eraseEntry(entryIndex);
        updateHints();
        return true;
      }
    }
    return false;
  } else {
    uint32_t childIndex = getEntryIndexByKey(key);
    return reinterpret_cast<BTreeInnerNodeBinarySearchHints *>(this)->getChild(childIndex)->remove(key);
  }
}
