#include "btreenode_template.hpp"
#include "utils.hpp"

BTreeNodeTemplate::BTreeNodeTemplate(std::span<uint8_t> lowerFenceKey, std::span<uint8_t> upperFenceKey) {
  uint16_t minLength = std::min(lowerFence.len, upperFence.len);
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
  uint16_t lowerFenceKeySize = lowerFence.len - prefixLen;
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
  uint16_t upperFenceKeySize = upperFence.len - prefixLen;
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

uint16_t BTreeNodeTemplate::getSplitIndex() {
  uint16_t splitIndex = 0;
  uint16_t spaceUsedSoFar = 0;
  for (uint16_t i = 0; i < numKeys; i++) {
    auto slot = reinterpret_cast<PageSlotTemplate *>(content + i * sizeof(PageSlotTemplate));
    uint16_t requiredSpace = slot->keyLength + slot->valueLength + sizeof(PageSlotTemplate);
    spaceUsedSoFar += requiredSpace;
    if (spaceUsedSoFar <= CONTENT_SIZE / 2) {
      splitIndex = i;
    } else {
      break;
    }
  }
  return std::max((uint16_t) 1, splitIndex);
}

std::vector<std::vector<uint8_t>> BTreeNodeTemplate::getKeys() {
  std::vector<std::vector<uint8_t>> keys(numKeys);
  for (uint16_t i = 0; i < numKeys; i++) {
    std::vector<uint8_t> key = getFullKey(i);
    keys[i] = key;
  }
  return keys;
}

std::vector<std::string> BTreeNodeTemplate::getKeysAsString() {
  std::vector<std::string> keys(numKeys);
  for (uint16_t i = 0; i < numKeys; i++) {
    std::vector<uint8_t> key = getFullKey(i);
    keys[i] = std::string(key.begin(), key.end());
  }
  return keys;
}

std::vector<std::string> BTreeNodeTemplate::getShortenedKeysAsString() {
  std::vector<std::string> keys(numKeys);
  for (uint16_t i = 0; i < numKeys; i++) {
    auto key = getShortenedKey(i);
    keys[i] = std::string(key.begin(), key.end());
  }
  return keys;
}

bool BTreeNodeTemplate::keySmallerEqualThanAtPosition(uint16_t position, uint32_t keyHead, std::span<uint8_t> key) {
  auto slot = reinterpret_cast<PageSlotTemplate *>(content + position * sizeof(PageSlotTemplate));
  if (keyHead < slot->keyHead) {
    return true;
  }
  if (keyHead > slot->keyHead) {
    return false;
  }

  // The key heads are equal, so we need to compare the full keys
  return key <= getShortenedKey(position);
}

void BTreeNodeTemplate::updateHints() {
  if (numKeys <= 16) {
    return;
  }

  uint16_t spacing = numKeys / 16;
  for (uint16_t i = 0; i < 16; i++) {
    auto slot = reinterpret_cast<PageSlotTemplate *>(content + i * spacing * sizeof(PageSlotTemplate));
    hints[i] = slot->keyHead;
  }
}

std::pair<int, int> BTreeNodeTemplate::getHintRange(uint32_t keyHead) {
  if (numKeys <= 16) {
    return std::make_pair(0, numKeys - 1);
  }
  uint16_t spacing = numKeys / 16;
  if (keyHead < hints[0]) {
    return std::make_pair(0, spacing - 1);
  }
  if (keyHead > hints[15]) {
    return std::make_pair(15 * spacing, numKeys - 1);
  }

  // Linear search on the hints to find the left and right boundary
  uint16_t left = 0;
  while (left + 1 < 16 && keyHead > hints[left + 1]) {
    left++;
  }
  uint16_t right = 15;
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

uint16_t BTreeNodeTemplate::getEntryIndexByKey(std::span<uint8_t> key) {
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
  uint16_t childIndex = right;

  while (left <= right) {
    uint16_t middle = (left + right) / 2;
    if (keySmallerEqualThanAtPosition(middle, keyHead, keyWithoutPrefix)) {
      childIndex = middle;
      right = middle - 1;
    } else {
      left = middle + 1;
    }
  }

  return childIndex;
}

void BTreeNodeTemplate::compact() {
  // Only the heap needs to be restructured, so compute where the heap starts
  const uint32_t heapStart = numKeys * sizeof(PageSlotTemplate);
  const uint32_t compactableSize = CONTENT_SIZE - heapStart;
  uint8_t *buffer = reinterpret_cast<uint8_t *>(alloca(compactableSize));
  uint32_t insertionOffset = compactableSize;

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
  for (uint16_t i = 0; i < numKeys; i++) {
    auto slot = reinterpret_cast<PageSlotTemplate *>(content + i * sizeof(PageSlotTemplate));
    uint16_t requiredSize = slot->keyLength + slot->valueLength;
    insertionOffset -= requiredSize;
    std::memcpy(buffer + insertionOffset, content + slot->offset, requiredSize);
    slot->offset = heapStart + insertionOffset;
  }

  // Copy the buffer back to the content, after the slots
  std::memcpy(content + heapStart, buffer, compactableSize);
  freeOffset = heapStart + insertionOffset;
}

void BTreeNodeTemplate::insertEntry(uint16_t position, std::span<uint8_t> key, std::span<uint8_t> value) {
  // First trim the key to get rid of the prefix
  std::span<uint8_t> keyWithoutPrefix = key.subspan(prefixLen);
  uint16_t requiredSpace = sizeof(PageSlotTemplate) + keyWithoutPrefix.size() + value.size();

  // Compact the node if there is enough space in the node, but not enough contiguous space in the heap
  bool overflows = freeOffset < requiredSpace || freeOffset - requiredSpace < static_cast<uint16_t>(numKeys * sizeof(PageSlotTemplate));
  if (overflows) {
    compact();
  }

  // Move all slots after the insertion position one slot to the right
  uint32_t moveFromOffset = position * sizeof(PageSlotTemplate);
  uint32_t moveToOffset = (position + 1) * sizeof(PageSlotTemplate);
  uint32_t moveSize = (numKeys - position) * sizeof(PageSlotTemplate);
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
  auto slot = reinterpret_cast<PageSlotTemplate *>(content + position * sizeof(PageSlotTemplate));
  slot->offset = freeOffset;
  slot->keyLength = keyWithoutPrefix.size();
  slot->valueLength = value.size();
  slot->keyHead = getKeyHead(keyWithoutPrefix);

  // Update the node state
  numKeys++;
  spaceUsed += requiredSpace;
}
void BTreeNodeTemplate::insertEntry(uint16_t position, std::span<uint8_t> key, BTreeNodeTemplate *childPointer) { insertEntry(position, key, std::span<uint8_t>(reinterpret_cast<uint8_t *>(&childPointer), sizeof(BTreeNodeTemplate *))); }

void BTreeNodeTemplate::eraseEntry(uint16_t position) {
  // If the rightmost child gets deleted, set the rightmost child to the child before it
  if (position == numKeys && !isLeaf) {
    rightMostChildTemplate = reinterpret_cast<BTreeInnerNodeTemplate *>(this)->getChild(position - 1);
    eraseEntry(position - 1);
    return;
  }

  // Update the node state
  auto slot = reinterpret_cast<PageSlotTemplate *>(content + position * sizeof(PageSlotTemplate));
  spaceUsed -= (sizeof(PageSlotTemplate) + slot->keyLength + slot->valueLength);

  // Move the slots after the deleted slot one slot to the left
  uint32_t moveFromOffset = (position + 1) * sizeof(PageSlotTemplate);
  uint32_t moveToOffset = position * sizeof(PageSlotTemplate);
  uint32_t moveSize = (numKeys - position - 1) * sizeof(PageSlotTemplate);
  std::memmove(content + moveToOffset, content + moveFromOffset, moveSize);

  numKeys--;
}

void BTreeInnerNodeTemplate::overwriteChild(uint16_t position, BTreeNodeTemplate *newChild) {
  auto slot = reinterpret_cast<PageSlotTemplate *>(content + position * sizeof(PageSlotTemplate));
  std::memcpy(content + slot->offset + slot->keyLength, &newChild, sizeof(BTreeNodeTemplate *));
}

void BTreeNodeTemplate::destroy() {
  if (isLeaf) {
    delete reinterpret_cast<BTreeLeafNodeTemplate *>(this);
  } else {
    BTreeInnerNodeTemplate *innerNode = reinterpret_cast<BTreeInnerNodeTemplate *>(this);
    for (uint16_t i = 0; i <= numKeys; ++i) {
      innerNode->getChild(i)->destroy();
    }
    delete innerNode;
  }
}

BTreeNodeTemplate *BTreeNodeTemplate::splitNode(uint16_t splitIndex, std::span<uint8_t> splitKey) {
  // Create the new node
  BTreeNodeTemplate *newNode = nullptr;
  auto upperFenceKey = getUpperFenceKey();
  if (isLeaf) {
    newNode = new BTreeLeafNodeTemplate(splitKey, upperFenceKey);
  } else {
    newNode = new BTreeInnerNodeTemplate(splitKey, upperFenceKey);
  }

  // Copy the entries starting from splitIndex+1 to the new node
  uint16_t initialEntryCount = numKeys;
  uint16_t insertIndex = 0;

  for (uint16_t i = splitIndex; i < initialEntryCount; ++i) {
    auto key = getFullKey(i);
    if (isLeaf) {
      auto value = reinterpret_cast<BTreeLeafNodeTemplate *>(this)->getValue(i);
      newNode->insertEntry(insertIndex, key, value);
    } else {
      auto child = reinterpret_cast<BTreeInnerNodeTemplate *>(this)->getChild(i);
      newNode->insertEntry(insertIndex, key, child);
    }
    ++insertIndex;
  }

  // If it's a leaf, update the linkedlist
  if (isLeaf) {
    newNode->nextLeafNodeTemplate = nextLeafNodeTemplate;
    nextLeafNodeTemplate = reinterpret_cast<BTreeLeafNodeTemplate *>(newNode);
  }

  return newNode;
}

std::optional<std::pair<BTreeNodeTemplate *, std::vector<uint8_t>>> BTreeNodeTemplate::insert(std::span<uint8_t> key, std::span<uint8_t> value) {
  if (!isLeaf) {
    uint16_t childIndex = getEntryIndexByKey(key);
    auto toInsert = reinterpret_cast<BTreeInnerNodeTemplate *>(this)->getChild(childIndex)->insert(key, value);
    if (!toInsert.has_value()) {
      return std::nullopt;
    }

    // Insert the new node
    BTreeNodeTemplate *nodeToInsert = toInsert->first;
    std::vector<uint8_t> keyToInsert = toInsert->second;
    uint16_t requiredSpace = sizeof(PageSlotTemplate) + keyToInsert.size() + sizeof(BTreeNodeTemplate *);
    bool canFit = spaceUsed + requiredSpace <= CONTENT_SIZE;
    BTreeNodeTemplate* currentChild = reinterpret_cast<BTreeInnerNodeTemplate *>(this)->getChild(childIndex);

    // If the entry can fit, insert it and then the insertion process is finished
    if (canFit) {
      if (childIndex == numKeys) {
        insertEntry(childIndex, keyToInsert, currentChild);
        reinterpret_cast<BTreeInnerNodeTemplate *>(this)->rightMostChildTemplate = nodeToInsert;
      } else {
        reinterpret_cast<BTreeInnerNodeTemplate *>(this)->overwriteChild(childIndex, nodeToInsert);
        insertEntry(childIndex, keyToInsert, currentChild);
      }
      updateHints();
      return std::nullopt;
    }

    // Split the node
    uint16_t splitIndex = getSplitIndex();
    uint16_t insertIndex = childIndex;
    if (splitIndex == insertIndex) {
      splitIndex--;
    }
    std::vector<uint8_t> splitKey = getFullKey(splitIndex);

    // Create the new right sibling, the new node with the entries [splitIndex+1, numKeys]
    BTreeInnerNodeTemplate *newRightSibling = reinterpret_cast<BTreeInnerNodeTemplate *>(splitNode(splitIndex + 1, splitKey));
    newRightSibling->rightMostChildTemplate = reinterpret_cast<BTreeInnerNodeTemplate *>(this)->rightMostChildTemplate;

    // Create a new node that will replace the current node because the prefixLen could change so all keys need to be reinserted
    auto lowerFenceKey = getLowerFenceKey();
    BTreeInnerNodeTemplate *newThisChild = new BTreeInnerNodeTemplate(lowerFenceKey, splitKey);

    // Insert all the entreis remaining in the current node into the new node
    for (uint16_t i = 0; i <= splitIndex; ++i) {
      auto key = getFullKey(i);
      auto child = reinterpret_cast<BTreeInnerNodeTemplate *>(this)->getChild(i);
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
        newRightSibling->rightMostChildTemplate = nodeToInsert;
      }

      else {
        newRightSibling->overwriteChild(insertIndex, nodeToInsert);
        newRightSibling->insertEntry(insertIndex, keyToInsert, currentChild);
      }
    }

    newThisChild->updateHints();
    newRightSibling->updateHints();

    // Swap the new node with the current one, delete the current one and return the new node to insert into the parent
    // with the corresponding split key
    std::swap(*reinterpret_cast<BTreeInnerNodeTemplate *>(this), *newThisChild);
    delete newThisChild;
    return std::make_pair(newRightSibling, splitKey);

  } else {
    std::span<uint8_t> keyWithoutPrefix = key.subspan(prefixLen);
    uint16_t insertIndex = getEntryIndexByKey(key);

    // Handle the case when the key already exists
    if (insertIndex < numKeys && keyWithoutPrefix == getShortenedKey(insertIndex)) {
      eraseEntry(insertIndex);
    }

    uint16_t requiredSpace = sizeof(PageSlotTemplate) + keyWithoutPrefix.size() + value.size();
    bool canFit = spaceUsed + requiredSpace <= CONTENT_SIZE;

    // If the key + value can fit in the current node, insert the entry
    if (canFit) {
      insertEntry(insertIndex, key, value);
      updateHints();
      return std::nullopt;
    }

    // Otherwise we need to split the node
    uint16_t splitIndex = getSplitIndex();
    std::vector<uint8_t> splitKey;
    BTreeLeafNodeTemplate *newRightSibling = nullptr;
    uint16_t lastEntryIndex;

    // Determine the correct splitKey and the new right sibling as well as the index of the last key that should remain
    // In the current node
    if (insertIndex < splitIndex) {
      splitKey = getFullKey(splitIndex - 1);
      newRightSibling = reinterpret_cast<BTreeLeafNodeTemplate *>(splitNode(splitIndex, splitKey));
      lastEntryIndex = splitIndex - 1;
    } else if (insertIndex > splitIndex) {
      splitKey = getFullKey(splitIndex);
      newRightSibling = reinterpret_cast<BTreeLeafNodeTemplate *>(splitNode(splitIndex + 1, splitKey));
      lastEntryIndex = splitIndex;
    } else {
      splitKey = std::vector<uint8_t>(key.begin(), key.end());
      newRightSibling = reinterpret_cast<BTreeLeafNodeTemplate *>(splitNode(splitIndex, splitKey));
      lastEntryIndex = splitIndex - 1;
    }

    // Create a new node that will replace the current node because the prefixLen could change so all keys need to be reinserted
    auto lowerFenceKey = getLowerFenceKey();
    BTreeLeafNodeTemplate *newThisChild = new BTreeLeafNodeTemplate(lowerFenceKey, splitKey);
    newThisChild->nextLeafNodeTemplate = newRightSibling;

    // Insert all the entreis remaining in the current node into the new node
    for (uint16_t i = 0; i <= lastEntryIndex; ++i) {
      auto key = getFullKey(i);
      auto value = reinterpret_cast<BTreeLeafNodeTemplate *>(this)->getValue(i);
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
    std::swap(*reinterpret_cast<BTreeLeafNodeTemplate *>(this), *newThisChild);
    delete newThisChild;
    return std::make_pair(newRightSibling, splitKey);
  }
}

bool BTreeNodeTemplate::remove(std::span<uint8_t> key) {
  if (isLeaf) {
    uint16_t entryIndex = getEntryIndexByKey(key);
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
    uint16_t childIndex = getEntryIndexByKey(key);
    return reinterpret_cast<BTreeInnerNodeTemplate *>(this)->getChild(childIndex)->remove(key);
  }
}
