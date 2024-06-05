#include "btreenode_binarySearch.hpp"
#include "utils.hpp"

#include <iostream>
#include <cmath>

BTreeNodeBinarySearch::BTreeNodeBinarySearch(std::span<uint8_t> lowerFenceKey, std::span<uint8_t> upperFenceKey) {
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

uint16_t BTreeNodeBinarySearch::getSplitIndex() {
  uint16_t splitIndex = 0;
  uint16_t spaceUsedSoFar = 0;
  for (uint16_t i = 0; i < numKeys; i++) {
    auto slot = reinterpret_cast<PageSlotBinarySearch *>(content + i * sizeof(PageSlotBinarySearch));
    uint16_t requiredSpace = slot->keyLength + slot->valueLength + sizeof(PageSlotBinarySearch);
    spaceUsedSoFar += requiredSpace;
    if (spaceUsedSoFar <= CONTENT_SIZE / 2) {
      splitIndex = i;
    } else {
      break;
    }
  }
  return std::max((uint16_t) 1, splitIndex);
}

std::vector<std::vector<uint8_t>> BTreeNodeBinarySearch::getKeys() {
  std::vector<std::vector<uint8_t>> keys(numKeys);
  for (uint16_t i = 0; i < numKeys; i++) {
    std::vector<uint8_t> key = getFullKey(i);
    keys[i] = key;
  }
  return keys;
}

std::vector<std::string> BTreeNodeBinarySearch::getKeysAsString() {
  std::vector<std::string> keys(numKeys);
  for (uint16_t i = 0; i < numKeys; i++) {
    std::vector<uint8_t> key = getFullKey(i);
    keys[i] = std::string(key.begin(), key.end());
  }
  return keys;
}

std::vector<std::string> BTreeNodeBinarySearch::getShortenedKeysAsString() {
  std::vector<std::string> keys(numKeys);
  for (uint16_t i = 0; i < numKeys; i++) {
    auto key = getShortenedKey(i);
    keys[i] = std::string(key.begin(), key.end());
  }
  return keys;
}

bool BTreeNodeBinarySearch::keySmallerEqualThanAtPosition(uint16_t position, uint32_t keyHead, std::span<uint8_t> key) {
  auto slot = reinterpret_cast<PageSlotBinarySearch *>(content + position * sizeof(PageSlotBinarySearch));
  if (keyHead < slot->keyHead) {
    return true;
  }
  if (keyHead > slot->keyHead) {
    return false;
  }

  // The key heads are equal, so we need to compare the full keys
  return key <= getShortenedKey(position);
}

std::vector<uint32_t> BTreeNodeBinarySearch::calculateKeyDifferences() {
  std::vector<uint32_t> keyDifferences;
  if (numKeys > 1) {
    for (uint64_t i = 1; i < numKeys; i++) {
      keyDifferences.push_back(getKeyHead(getShortenedKey(i)) - getKeyHead(getShortenedKey(i-1)));
    }
  } else {
    keyDifferences.push_back(0);
  }
  return keyDifferences;
}

void BTreeNodeBinarySearch::analyzeInnerNodes(std::vector<double> &coefficientOfVariation) {
  if (!isLeaf) {
    BTreeInnerNodeBinarySearch *innerNode = reinterpret_cast<BTreeInnerNodeBinarySearch *>(this);
    for (uint16_t i = 0; i <= numKeys; ++i) {
      innerNode->getChild(i)->analyzeInnerNodes(coefficientOfVariation);
    }
    std::vector<uint32_t> keyDifferences = calculateKeyDifferences();
    double mean = this->mean(keyDifferences);
    double standardDeviation = this->standardDeviation(keyDifferences, mean);
    double coefficient = standardDeviation / mean;
    coefficientOfVariation.push_back(coefficient);
  }
}

void BTreeNodeBinarySearch::numKeysInnerNodes(std::vector<uint32_t> &numKeysInInnerNodes) {
  if (!isLeaf) {
    BTreeInnerNodeBinarySearch *innerNode = reinterpret_cast<BTreeInnerNodeBinarySearch *>(this);
    for (uint16_t i = 0; i <= numKeys; ++i) {
      innerNode->getChild(i)->numKeysInnerNodes(numKeysInInnerNodes);
    }
    uint32_t numKeysInCurrentInnerNode = this->numKeys;
    numKeysInInnerNodes.push_back(numKeysInCurrentInnerNode);
  }
}

void BTreeNodeBinarySearch::analyzeLeafs(std::vector<double> &coefficientOfVariation) {
  if (!isLeaf) {
    reinterpret_cast<BTreeInnerNodeBinarySearch *>(this)->getChild(0)->analyzeLeafs(coefficientOfVariation);
  } else {
    std::vector<uint32_t> keyDifferences = calculateKeyDifferences();
    double mean = this->mean(keyDifferences);
    double standardDeviation = this->standardDeviation(keyDifferences, mean);
    double coefficient = standardDeviation / mean;
    keyDifferences.push_back(coefficient);
    if(nextLeafNodeBinarySearch != nullptr) {
      nextLeafNodeBinarySearch->analyzeLeafs(coefficientOfVariation);
    }
  }
}

double BTreeNodeBinarySearch::mean(std::vector<uint32_t> keyDifferences) {
  if (keyDifferences.size() > 0) {
    double total = 0.0;
    double direkt = 0.0;
    for (u_int64_t i = 0; i < keyDifferences.size(); i++) {
      total += keyDifferences.at(i);
      direkt += static_cast<double>(keyDifferences.at(i)) / static_cast<double>(keyDifferences.size());
    }
    double mean = total / static_cast<double>(keyDifferences.size());
    double mean2 = direkt;
    if (mean - mean2 > 1 || mean2 - mean > 1) {
      total++;
    }
    return mean;
  }
  return 0.0;
}

double BTreeNodeBinarySearch::standardDeviation(std::vector<uint32_t> keyDifferences, double mean) {
  double tempResult = 0.0;
  for (uint32_t diff : keyDifferences) {
    tempResult += std::pow(diff - mean, 2);
  }
  double variance = tempResult / keyDifferences.size();
  double standardDeviation = std::sqrt(variance);
  return standardDeviation;
}

uint16_t BTreeNodeBinarySearch::getEntryIndexByKey(std::span<uint8_t> key) {
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

  uint16_t left = 0;
  uint16_t right = numKeys - 1;
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

void BTreeNodeBinarySearch::compact() {
  // Only the heap needs to be restructured, so compute where the heap starts
  const uint16_t heapStart = numKeys * sizeof(PageSlotBinarySearch);
  const uint16_t compactableSize = CONTENT_SIZE - heapStart;
  uint8_t *buffer = reinterpret_cast<uint8_t *>(alloca(compactableSize));
  uint16_t insertionOffset = compactableSize;

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
  for (uint16_t i = 0; i < numKeys; i++) {
    auto slot = reinterpret_cast<PageSlotBinarySearch *>(content + i * sizeof(PageSlotBinarySearch));
    uint16_t requiredSize = slot->keyLength + slot->valueLength;
    insertionOffset -= requiredSize;
    std::memcpy(buffer + insertionOffset, content + slot->offset, requiredSize);
    slot->offset = heapStart + insertionOffset;
  }

  // Copy the buffer back to the content, after the slots
  std::memcpy(content + heapStart, buffer, compactableSize);
  freeOffset = heapStart + insertionOffset;
}

void BTreeNodeBinarySearch::insertEntry(uint16_t position, std::span<uint8_t> key, std::span<uint8_t> value) {
  // First trim the key to get rid of the prefix
  std::span<uint8_t> keyWithoutPrefix = key.subspan(prefixLen);
  uint16_t requiredSpace = sizeof(PageSlotBinarySearch) + keyWithoutPrefix.size() + value.size();

  // Compact the node if there is enough space in the node, but not enough contiguous space in the heap
  bool overflows = freeOffset < requiredSpace || freeOffset - requiredSpace < static_cast<uint16_t>(numKeys * sizeof(PageSlotBinarySearch));
  if (overflows) {
    compact();
  }

  // Move all slots after the insertion position one slot to the right
  uint16_t moveFromOffset = position * sizeof(PageSlotBinarySearch);
  uint16_t moveToOffset = (position + 1) * sizeof(PageSlotBinarySearch);
  uint16_t moveSize = (numKeys - position) * sizeof(PageSlotBinarySearch);
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
  auto slot = reinterpret_cast<PageSlotBinarySearch *>(content + position * sizeof(PageSlotBinarySearch));
  slot->offset = freeOffset;
  slot->keyLength = keyWithoutPrefix.size();
  slot->valueLength = value.size();
  slot->keyHead = getKeyHead(keyWithoutPrefix);

  // Update the node state
  numKeys++;
  spaceUsed += requiredSpace;
}
void BTreeNodeBinarySearch::insertEntry(uint16_t position, std::span<uint8_t> key, BTreeNodeBinarySearch *childPointer) { insertEntry(position, key, std::span<uint8_t>(reinterpret_cast<uint8_t *>(&childPointer), sizeof(BTreeNodeBinarySearch *))); }

void BTreeNodeBinarySearch::eraseEntry(uint16_t position) {
  // If the rightmost child gets deleted, set the rightmost child to the child before it
  if (position == numKeys && !isLeaf) {
    rightMostChildBinarySearch = reinterpret_cast<BTreeInnerNodeBinarySearch *>(this)->getChild(position - 1);
    eraseEntry(position - 1);
    return;
  }

  // Update the node state
  auto slot = reinterpret_cast<PageSlotBinarySearch *>(content + position * sizeof(PageSlotBinarySearch));
  spaceUsed -= (sizeof(PageSlotBinarySearch) + slot->keyLength + slot->valueLength);

  // Move the slots after the deleted slot one slot to the left
  uint16_t moveFromOffset = (position + 1) * sizeof(PageSlotBinarySearch);
  uint16_t moveToOffset = position * sizeof(PageSlotBinarySearch);
  uint16_t moveSize = (numKeys - position - 1) * sizeof(PageSlotBinarySearch);
  std::memmove(content + moveToOffset, content + moveFromOffset, moveSize);

  numKeys--;
}

void BTreeInnerNodeBinarySearch::overwriteChild(uint16_t position, BTreeNodeBinarySearch *newChild) {
  auto slot = reinterpret_cast<PageSlotBinarySearch *>(content + position * sizeof(PageSlotBinarySearch));
  std::memcpy(content + slot->offset + slot->keyLength, &newChild, sizeof(BTreeNodeBinarySearch *));
}

void BTreeNodeBinarySearch::destroy() {
  if (isLeaf) {
    delete reinterpret_cast<BTreeLeafNodeBinarySearch *>(this);
  } else {
    BTreeInnerNodeBinarySearch *innerNode = reinterpret_cast<BTreeInnerNodeBinarySearch *>(this);
    for (uint16_t i = 0; i <= numKeys; ++i) {
      innerNode->getChild(i)->destroy();
    }
    delete innerNode;
  }
}

BTreeNodeBinarySearch *BTreeNodeBinarySearch::splitNode(uint16_t splitIndex, std::span<uint8_t> splitKey) {
  // Create the new node
  BTreeNodeBinarySearch *newNode = nullptr;
  auto upperFenceKey = getUpperFenceKey();
  if (isLeaf) {
    newNode = new BTreeLeafNodeBinarySearch(splitKey, upperFenceKey);
  } else {
    newNode = new BTreeInnerNodeBinarySearch(splitKey, upperFenceKey);
  }

  // Copy the entries starting from splitIndex+1 to the new node
  uint16_t initialEntryCount = numKeys;
  uint16_t insertIndex = 0;

  for (uint16_t i = splitIndex; i < initialEntryCount; ++i) {
    auto key = getFullKey(i);
    if (isLeaf) {
      auto value = reinterpret_cast<BTreeLeafNodeBinarySearch *>(this)->getValue(i);
      newNode->insertEntry(insertIndex, key, value);
    } else {
      auto child = reinterpret_cast<BTreeInnerNodeBinarySearch *>(this)->getChild(i);
      newNode->insertEntry(insertIndex, key, child);
    }
    ++insertIndex;
  }

  // If it's a leaf, update the linkedlist
  if (isLeaf) {
    newNode->nextLeafNodeBinarySearch = nextLeafNodeBinarySearch;
    nextLeafNodeBinarySearch = reinterpret_cast<BTreeLeafNodeBinarySearch *>(newNode);
  }

  return newNode;
}

std::optional<std::pair<BTreeNodeBinarySearch *, std::vector<uint8_t>>> BTreeNodeBinarySearch::insert(std::span<uint8_t> key, std::span<uint8_t> value) {
  if (!isLeaf) {
    uint16_t childIndex = getEntryIndexByKey(key);
    auto toInsert = reinterpret_cast<BTreeInnerNodeBinarySearch *>(this)->getChild(childIndex)->insert(key, value);
    if (!toInsert.has_value()) {
      return std::nullopt;
    }

    // Insert the new node
    BTreeNodeBinarySearch *nodeToInsert = toInsert->first;
    std::vector<uint8_t> keyToInsert = toInsert->second;
    uint16_t requiredSpace = sizeof(PageSlotBinarySearch) + keyToInsert.size() + sizeof(BTreeNodeBinarySearch *);
    bool canFit = spaceUsed + requiredSpace <= CONTENT_SIZE;
    BTreeNodeBinarySearch* currentChild = reinterpret_cast<BTreeInnerNodeBinarySearch *>(this)->getChild(childIndex);

    // If the entry can fit, insert it and then the insertion process is finished
    if (canFit) {
      if (childIndex == numKeys) {
        insertEntry(childIndex, keyToInsert, currentChild);
        reinterpret_cast<BTreeInnerNodeBinarySearch *>(this)->rightMostChildBinarySearch = nodeToInsert;
      } else {
        reinterpret_cast<BTreeInnerNodeBinarySearch *>(this)->overwriteChild(childIndex, nodeToInsert);
        insertEntry(childIndex, keyToInsert, currentChild);
      }
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
    BTreeInnerNodeBinarySearch *newRightSibling = reinterpret_cast<BTreeInnerNodeBinarySearch *>(splitNode(splitIndex + 1, splitKey));
    newRightSibling->rightMostChildBinarySearch = reinterpret_cast<BTreeInnerNodeBinarySearch *>(this)->rightMostChildBinarySearch;

    // Create a new node that will replace the current node because the prefixLen could change so all keys need to be reinserted
    auto lowerFenceKey = getLowerFenceKey();
    BTreeInnerNodeBinarySearch *newThisChild = new BTreeInnerNodeBinarySearch(lowerFenceKey, splitKey);

    // Insert all the entreis remaining in the current node into the new node
    for (uint16_t i = 0; i <= splitIndex; ++i) {
      auto key = getFullKey(i);
      auto child = reinterpret_cast<BTreeInnerNodeBinarySearch *>(this)->getChild(i);
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
        newRightSibling->rightMostChildBinarySearch = nodeToInsert;
      }

      else {
        newRightSibling->overwriteChild(insertIndex, nodeToInsert);
        newRightSibling->insertEntry(insertIndex, keyToInsert, currentChild);
      }
    }

    // Swap the new node with the current one, delete the current one and return the new node to insert into the parent
    // with the corresponding split key
    std::swap(*reinterpret_cast<BTreeInnerNodeBinarySearch *>(this), *newThisChild);
    delete newThisChild;
    return std::make_pair(newRightSibling, splitKey);

  } else {
    std::span<uint8_t> keyWithoutPrefix = key.subspan(prefixLen);
    uint16_t insertIndex = getEntryIndexByKey(key);

    // Handle the case when the key already exists
    if (insertIndex < numKeys && keyWithoutPrefix == getShortenedKey(insertIndex)) {
      eraseEntry(insertIndex);
    }

    uint16_t requiredSpace = sizeof(PageSlotBinarySearch) + keyWithoutPrefix.size() + value.size();
    bool canFit = spaceUsed + requiredSpace <= CONTENT_SIZE;

    // If the key + value can fit in the current node, insert the entry
    if (canFit) {
      insertEntry(insertIndex, key, value);
      return std::nullopt;
    }

    // Otherwise we need to split the node
    uint16_t splitIndex = getSplitIndex();
    std::vector<uint8_t> splitKey;
    BTreeLeafNodeBinarySearch *newRightSibling = nullptr;
    uint16_t lastEntryIndex;

    // Determine the correct splitKey and the new right sibling as well as the index of the last key that should remain
    // In the current node
    if (insertIndex < splitIndex) {
      splitKey = getFullKey(splitIndex - 1);
      newRightSibling = reinterpret_cast<BTreeLeafNodeBinarySearch *>(splitNode(splitIndex, splitKey));
      lastEntryIndex = splitIndex - 1;
    } else if (insertIndex > splitIndex) {
      splitKey = getFullKey(splitIndex);
      newRightSibling = reinterpret_cast<BTreeLeafNodeBinarySearch *>(splitNode(splitIndex + 1, splitKey));
      lastEntryIndex = splitIndex;
    } else {
      splitKey = std::vector<uint8_t>(key.begin(), key.end());
      newRightSibling = reinterpret_cast<BTreeLeafNodeBinarySearch *>(splitNode(splitIndex, splitKey));
      lastEntryIndex = splitIndex - 1;
    }

    // Create a new node that will replace the current node because the prefixLen could change so all keys need to be reinserted
    auto lowerFenceKey = getLowerFenceKey();
    BTreeLeafNodeBinarySearch *newThisChild = new BTreeLeafNodeBinarySearch(lowerFenceKey, splitKey);
    newThisChild->nextLeafNodeBinarySearch = newRightSibling;

    // Insert all the entreis remaining in the current node into the new node
    for (uint16_t i = 0; i <= lastEntryIndex; ++i) {
      auto key = getFullKey(i);
      auto value = reinterpret_cast<BTreeLeafNodeBinarySearch *>(this)->getValue(i);
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
    std::swap(*reinterpret_cast<BTreeLeafNodeBinarySearch *>(this), *newThisChild);
    delete newThisChild;
    return std::make_pair(newRightSibling, splitKey);
  }
}

bool BTreeNodeBinarySearch::remove(std::span<uint8_t> key) {
  if (isLeaf) {
    uint16_t entryIndex = getEntryIndexByKey(key);
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
    uint16_t childIndex = getEntryIndexByKey(key);
    return reinterpret_cast<BTreeInnerNodeBinarySearch *>(this)->getChild(childIndex)->remove(key);
  }
}
