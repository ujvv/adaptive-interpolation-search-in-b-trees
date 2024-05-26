#include "btreenode_interpolationSearchBigNode.hpp"
#include "utils.hpp"

#include <iostream>
#include <cmath>
#include <vector>

BTreeNodeInterpolationSearchBigNode::BTreeNodeInterpolationSearchBigNode(std::span<uint8_t> lowerFenceKey, std::span<uint8_t> upperFenceKey) {
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

uint16_t BTreeNodeInterpolationSearchBigNode::getSplitIndex() {
  uint16_t splitIndex = 0;
  uint16_t spaceUsedSoFar = 0;
  for (uint16_t i = 0; i < numKeys; i++) {
    auto slot = reinterpret_cast<PageSlotInterpolationSearchBigNode *>(content + i * sizeof(PageSlotInterpolationSearchBigNode));
    uint16_t requiredSpace = slot->keyLength + slot->valueLength + sizeof(PageSlotInterpolationSearchBigNode);
    spaceUsedSoFar += requiredSpace;
    if (spaceUsedSoFar <= CONTENT_SIZE / 2) {
      splitIndex = i;
    } else {
      break;
    }
  }
  return std::max((uint16_t) 1, splitIndex);
}

std::vector<std::vector<uint8_t>> BTreeNodeInterpolationSearchBigNode::getKeys() {
  std::vector<std::vector<uint8_t>> keys(numKeys);
  for (uint16_t i = 0; i < numKeys; i++) {
    std::vector<uint8_t> key = getFullKey(i);
    keys[i] = key;
  }
  return keys;
}

std::vector<std::string> BTreeNodeInterpolationSearchBigNode::getKeysAsString() {
  std::vector<std::string> keys(numKeys);
  for (uint16_t i = 0; i < numKeys; i++) {
    std::vector<uint8_t> key = getFullKey(i);
    keys[i] = std::string(key.begin(), key.end());
  }
  return keys;
}

std::vector<std::string> BTreeNodeInterpolationSearchBigNode::getShortenedKeysAsString() {
  std::vector<std::string> keys(numKeys);
  for (uint16_t i = 0; i < numKeys; i++) {
    auto key = getShortenedKey(i);
    keys[i] = std::string(key.begin(), key.end());
  }
  return keys;
}

bool BTreeNodeInterpolationSearchBigNode::keySmallerEqualThanAtPosition(uint16_t position, uint32_t keyHead, std::span<uint8_t> key) {
  auto slot = reinterpret_cast<PageSlotInterpolationSearchBigNode *>(content + position * sizeof(PageSlotInterpolationSearchBigNode));
  if (keyHead < slot->keyHead) {
    return true;
  }
  if (keyHead > slot->keyHead) {
    return false;
  }

  // The key heads are equal, so we need to compare the full keys
  return key <= getShortenedKey(position);
}

bool BTreeNodeInterpolationSearchBigNode::keySmallerThanAtPosition(uint16_t position, uint32_t keyHead, std::span<uint8_t> key) {
  auto slot = reinterpret_cast<PageSlotInterpolationSearchBigNode *>(content + position * sizeof(PageSlotInterpolationSearchBigNode));
  if (keyHead < slot->keyHead) {
    return true;
  }
  if (keyHead > slot->keyHead) {
    return false;
  }

  // The key heads are equal, so we need to compare the full keys
  return key < getShortenedKey(position);
}

uint16_t BTreeNodeInterpolationSearchBigNode::getEntryIndexByKey(std::span<uint8_t> key) {
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


  uint16_t left = 0;
  uint16_t right = numKeys - 1;
  uint16_t childIndex = right;
  uint16_t next; // Interpolation "guessed" key-index

  while (left < right) {
    // Interpolation Search Calculation:
    // next = (key - getKey(left)) / (getKey(right) - getKey(left)) * (right - left) + left
    
    // Dividend Calculation
    std::vector<int> dividendVector;
    dividendVector.reserve(6); // Extracting Max 6 bytes out of key for Interpolation Calculation
    int dividendLeadingZeros = 0; // Sum of the leading zeros of the byte sequence after Subtraction
    for (int i = 0; dividendVector.size() < 6 && i < std::min(getShortenedKey(left).size(), keyWithoutPrefix.size()); i++) {
        int value = static_cast<int>(keyWithoutPrefix[i]) - static_cast<int>(getShortenedKey(left)[i]);
        if (dividendVector.size() == 0 && value == 0) {
          continue; // Skip the bytes at the beginning that are zero
        }
        if (dividendVector.size() == 0) {
          if (value < 0) { // if leftKey > key
            return left; // means correct index was found in previous iteration
          }
          dividendLeadingZeros = i;
        }
        dividendVector.push_back(value);
    }
    if (getShortenedKey(left).size() < keyWithoutPrefix.size()) {
      for (int i = std::min(getShortenedKey(left).size(), keyWithoutPrefix.size()); dividendVector.size() < 6 && i < keyWithoutPrefix.size(); i++) {
        int value = static_cast<int>(keyWithoutPrefix[i]);
        if (dividendVector.size() == 0 && value == 0) {
          continue; // Skip the bytes at the beginning that are zero
        }
        if (dividendVector.size() == 0) {
          if (value < 0) { // if leftKey > key
            return left;
          }
          dividendLeadingZeros = i;
        }
        dividendVector.push_back(value);
      }
    } else if(getShortenedKey(left).size() > keyWithoutPrefix.size()) {
      for (int i = std::min(getShortenedKey(left).size(), keyWithoutPrefix.size()); dividendVector.size() < 6 && i < getShortenedKey(left).size(); i++) {
        int value = 0 - static_cast<int>(getShortenedKey(left)[i]);
        if (dividendVector.size() == 0 && value == 0) {
          continue; // Skip the bytes at the beginning that are zero
        }
        if (dividendVector.size() == 0) {
          if (value < 0) { // if leftKey > key
            return left;
          }
          dividendLeadingZeros = i;
        }
        dividendVector.push_back(value);
      }
    }
    uint64_t dividendDecimal = 0;
    for (int i = 0; i < dividendVector.size(); i++) {
      dividendDecimal += dividendVector.at(i) * std::pow(256, 6 - i);
    }

    // Divisor Calculation
    std::vector<uint8_t> divisorVector;
    divisorVector.reserve(6); // Extracting Max 6 bytes out of key for Interpolation Calculation
    int divisorLeadingZeros = 0; // Sum of the leading zeros of the byte sequence after Subtraction
    for (int i = 0; divisorVector.size() < 6 && i < std::min(getShortenedKey(left).size(), getShortenedKey(right).size()); i++) {
        int value = static_cast<int>(getShortenedKey(right)[i]) - static_cast<int>(getShortenedKey(left)[i]);
        if (divisorVector.size() == 0 && value == 0) {
          continue; // Skip the bytes at the beginning that are zero
        }
        if (divisorVector.size() == 0) {
          divisorLeadingZeros = i;
        }
        divisorVector.push_back(value);
    }
    if (getShortenedKey(left).size() < getShortenedKey(right).size()) {
      for (int i = std::min(getShortenedKey(left).size(), getShortenedKey(right).size()); divisorVector.size() < 6 && i < getShortenedKey(right).size(); i++) {
        int value = static_cast<int>(getShortenedKey(right)[i]);
        if (divisorVector.size() == 0 && value == 0) {
          continue; // Skip the bytes at the beginning that are zero
        }
        if (divisorVector.size() == 0) {
          divisorLeadingZeros = i;
        }
        divisorVector.push_back(value);
      }
    } else if(getShortenedKey(left).size() > getShortenedKey(right).size()) {
      for (int i = std::min(getShortenedKey(left).size(), getShortenedKey(right).size()); divisorVector.size() < 6 && i < getShortenedKey(left).size(); i++) {
        int value = 0 - static_cast<int>(getShortenedKey(left)[i]);
        if (divisorVector.size() == 0 && value == 0) {
          continue; // Skip the bytes at the beginning that are zero
        }
        if (divisorVector.size() == 0) {
          divisorLeadingZeros = i;
        }
        divisorVector.push_back(value);
      }
    }
    uint64_t divisorDecimal = 0;
    for (int i = 0; i < divisorVector.size(); i++) {
      divisorDecimal += divisorVector.at(i) * std::pow(256, 6 - i);
    }
    if (divisorDecimal == 0) {
      return childIndex; // Edge Case: When getShortenedKey(1) is 0
    }

    // Final Interpolation Calculations
    double quotient = static_cast<double>(dividendDecimal) / static_cast<double>(divisorDecimal);
    quotient = quotient / std::pow(256, dividendLeadingZeros - divisorLeadingZeros); // Adjust for possible byte shift between divisor and dividend vector (caused by leading zeros from subtraction)
    if (quotient > 1) {
      return childIndex;
    }
    next = quotient * (right - left) + left;

    // Compare new interpolation next-index with key
    if (keySmallerEqualThanAtPosition(next, keyHead, keyWithoutPrefix)) {
      childIndex = next;
      right = next - 1;
    } else {
      left = next + 1;
    }
  } // end while
  if (left == right && keySmallerEqualThanAtPosition(left, keyHead, keyWithoutPrefix)) {
    return left;
  }
  return childIndex;
}

void BTreeNodeInterpolationSearchBigNode::compact() {
  // Only the heap needs to be restructured, so compute where the heap starts
  const uint16_t heapStart = numKeys * sizeof(PageSlotInterpolationSearchBigNode);
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
    auto slot = reinterpret_cast<PageSlotInterpolationSearchBigNode *>(content + i * sizeof(PageSlotInterpolationSearchBigNode));
    uint16_t requiredSize = slot->keyLength + slot->valueLength;
    insertionOffset -= requiredSize;
    std::memcpy(buffer + insertionOffset, content + slot->offset, requiredSize);
    slot->offset = heapStart + insertionOffset;
  }

  // Copy the buffer back to the content, after the slots
  std::memcpy(content + heapStart, buffer, compactableSize);
  freeOffset = heapStart + insertionOffset;
}

void BTreeNodeInterpolationSearchBigNode::insertEntry(uint16_t position, std::span<uint8_t> key, std::span<uint8_t> value) {
  // First trim the key to get rid of the prefix
  std::span<uint8_t> keyWithoutPrefix = key.subspan(prefixLen);
  uint16_t requiredSpace = sizeof(PageSlotInterpolationSearchBigNode) + keyWithoutPrefix.size() + value.size();

  // Compact the node if there is enough space in the node, but not enough contiguous space in the heap
  bool overflows = freeOffset < requiredSpace || freeOffset - requiredSpace < static_cast<uint16_t>(numKeys * sizeof(PageSlotInterpolationSearchBigNode));
  if (overflows) {
    compact();
  }

  // Move all slots after the insertion position one slot to the right
  uint16_t moveFromOffset = position * sizeof(PageSlotInterpolationSearchBigNode);
  uint16_t moveToOffset = (position + 1) * sizeof(PageSlotInterpolationSearchBigNode);
  uint16_t moveSize = (numKeys - position) * sizeof(PageSlotInterpolationSearchBigNode);
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
  auto slot = reinterpret_cast<PageSlotInterpolationSearchBigNode *>(content + position * sizeof(PageSlotInterpolationSearchBigNode));
  slot->offset = freeOffset;
  slot->keyLength = keyWithoutPrefix.size();
  slot->valueLength = value.size();
  slot->keyHead = getKeyHead(keyWithoutPrefix);

  // Update the node state
  numKeys++;
  spaceUsed += requiredSpace;
}
void BTreeNodeInterpolationSearchBigNode::insertEntry(uint16_t position, std::span<uint8_t> key, BTreeNodeInterpolationSearchBigNode *childPointer) { insertEntry(position, key, std::span<uint8_t>(reinterpret_cast<uint8_t *>(&childPointer), sizeof(BTreeNodeInterpolationSearchBigNode *))); }

void BTreeNodeInterpolationSearchBigNode::eraseEntry(uint16_t position) {
  // If the rightmost child gets deleted, set the rightmost child to the child before it
  if (position == numKeys && !isLeaf) {
    rightMostChildInterpolationSearchBigNode = reinterpret_cast<BTreeInnerNodeInterpolationSearchBigNode *>(this)->getChild(position - 1);
    eraseEntry(position - 1);
    return;
  }

  // Update the node state
  auto slot = reinterpret_cast<PageSlotInterpolationSearchBigNode *>(content + position * sizeof(PageSlotInterpolationSearchBigNode));
  spaceUsed -= (sizeof(PageSlotInterpolationSearchBigNode) + slot->keyLength + slot->valueLength);

  // Move the slots after the deleted slot one slot to the left
  uint16_t moveFromOffset = (position + 1) * sizeof(PageSlotInterpolationSearchBigNode);
  uint16_t moveToOffset = position * sizeof(PageSlotInterpolationSearchBigNode);
  uint16_t moveSize = (numKeys - position - 1) * sizeof(PageSlotInterpolationSearchBigNode);
  std::memmove(content + moveToOffset, content + moveFromOffset, moveSize);

  numKeys--;
}

void BTreeInnerNodeInterpolationSearchBigNode::overwriteChild(uint16_t position, BTreeNodeInterpolationSearchBigNode *newChild) {
  auto slot = reinterpret_cast<PageSlotInterpolationSearchBigNode *>(content + position * sizeof(PageSlotInterpolationSearchBigNode));
  std::memcpy(content + slot->offset + slot->keyLength, &newChild, sizeof(BTreeNodeInterpolationSearchBigNode *));
}

void BTreeNodeInterpolationSearchBigNode::destroy() {
  if (isLeaf) {
    delete reinterpret_cast<BTreeLeafNodeInterpolationSearchBigNode *>(this);
  } else {
    BTreeInnerNodeInterpolationSearchBigNode *innerNode = reinterpret_cast<BTreeInnerNodeInterpolationSearchBigNode *>(this);
    for (uint16_t i = 0; i <= numKeys; ++i) {
      innerNode->getChild(i)->destroy();
    }
    delete innerNode;
  }
}

BTreeNodeInterpolationSearchBigNode *BTreeNodeInterpolationSearchBigNode::splitNode(uint16_t splitIndex, std::span<uint8_t> splitKey) {
  // Create the new node
  BTreeNodeInterpolationSearchBigNode *newNode = nullptr;
  auto upperFenceKey = getUpperFenceKey();
  if (isLeaf) {
    newNode = new BTreeLeafNodeInterpolationSearchBigNode(splitKey, upperFenceKey);
  } else {
    newNode = new BTreeInnerNodeInterpolationSearchBigNode(splitKey, upperFenceKey);
  }

  // Copy the entries starting from splitIndex+1 to the new node
  uint16_t initialEntryCount = numKeys;
  uint16_t insertIndex = 0;

  for (uint16_t i = splitIndex; i < initialEntryCount; ++i) {
    auto key = getFullKey(i);
    if (isLeaf) {
      auto value = reinterpret_cast<BTreeLeafNodeInterpolationSearchBigNode *>(this)->getValue(i);
      newNode->insertEntry(insertIndex, key, value);
    } else {
      auto child = reinterpret_cast<BTreeInnerNodeInterpolationSearchBigNode *>(this)->getChild(i);
      newNode->insertEntry(insertIndex, key, child);
    }
    ++insertIndex;
  }

  // If it's a leaf, update the linkedlist
  if (isLeaf) {
    newNode->nextLeafNodeInterpolationSearchBigNode = nextLeafNodeInterpolationSearchBigNode;
    nextLeafNodeInterpolationSearchBigNode = reinterpret_cast<BTreeLeafNodeInterpolationSearchBigNode *>(newNode);
  }

  return newNode;
}

std::optional<std::pair<BTreeNodeInterpolationSearchBigNode *, std::vector<uint8_t>>> BTreeNodeInterpolationSearchBigNode::insert(std::span<uint8_t> key, std::span<uint8_t> value) {
  if (!isLeaf) {
    uint16_t childIndex = getEntryIndexByKey(key);
    auto toInsert = reinterpret_cast<BTreeInnerNodeInterpolationSearchBigNode *>(this)->getChild(childIndex)->insert(key, value);
    if (!toInsert.has_value()) {
      return std::nullopt;
    }

    // Insert the new node
    BTreeNodeInterpolationSearchBigNode *nodeToInsert = toInsert->first;
    std::vector<uint8_t> keyToInsert = toInsert->second;
    uint16_t requiredSpace = sizeof(PageSlotInterpolationSearchBigNode) + keyToInsert.size() + sizeof(BTreeNodeInterpolationSearchBigNode *);
    bool canFit = spaceUsed + requiredSpace <= CONTENT_SIZE;
    BTreeNodeInterpolationSearchBigNode* currentChild = reinterpret_cast<BTreeInnerNodeInterpolationSearchBigNode *>(this)->getChild(childIndex);

    // If the entry can fit, insert it and then the insertion process is finished
    if (canFit) {
      if (childIndex == numKeys) {
        insertEntry(childIndex, keyToInsert, currentChild);
        reinterpret_cast<BTreeInnerNodeInterpolationSearchBigNode *>(this)->rightMostChildInterpolationSearchBigNode = nodeToInsert;
      } else {
        reinterpret_cast<BTreeInnerNodeInterpolationSearchBigNode *>(this)->overwriteChild(childIndex, nodeToInsert);
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
    BTreeInnerNodeInterpolationSearchBigNode *newRightSibling = reinterpret_cast<BTreeInnerNodeInterpolationSearchBigNode *>(splitNode(splitIndex + 1, splitKey));
    newRightSibling->rightMostChildInterpolationSearchBigNode = reinterpret_cast<BTreeInnerNodeInterpolationSearchBigNode *>(this)->rightMostChildInterpolationSearchBigNode;

    // Create a new node that will replace the current node because the prefixLen could change so all keys need to be reinserted
    auto lowerFenceKey = getLowerFenceKey();
    BTreeInnerNodeInterpolationSearchBigNode *newThisChild = new BTreeInnerNodeInterpolationSearchBigNode(lowerFenceKey, splitKey);

    // Insert all the entreis remaining in the current node into the new node
    for (uint16_t i = 0; i <= splitIndex; ++i) {
      auto key = getFullKey(i);
      auto child = reinterpret_cast<BTreeInnerNodeInterpolationSearchBigNode *>(this)->getChild(i);
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
        newRightSibling->rightMostChildInterpolationSearchBigNode = nodeToInsert;
      } else {
        newRightSibling->overwriteChild(insertIndex, nodeToInsert);
        newRightSibling->insertEntry(insertIndex, keyToInsert, currentChild);
      }
    }

    // Swap the new node with the current one, delete the current one and return the new node to insert into the parent
    // with the corresponding split key
    std::swap(*reinterpret_cast<BTreeInnerNodeInterpolationSearchBigNode *>(this), *newThisChild);
    delete newThisChild;
    return std::make_pair(newRightSibling, splitKey);

  } else {
    std::span<uint8_t> keyWithoutPrefix = key.subspan(prefixLen);
    uint16_t insertIndex = getEntryIndexByKey(key);

    // Handle the case when the key already exists
    if (insertIndex < numKeys && keyWithoutPrefix == getShortenedKey(insertIndex)) {
      eraseEntry(insertIndex);
    }

    uint16_t requiredSpace = sizeof(PageSlotInterpolationSearchBigNode) + keyWithoutPrefix.size() + value.size();
    bool canFit = spaceUsed + requiredSpace <= CONTENT_SIZE;

    // If the key + value can fit in the current node, insert the entry
    if (canFit) {
      insertEntry(insertIndex, key, value);
      return std::nullopt;
    }

    // Otherwise we need to split the node
    uint16_t splitIndex = getSplitIndex();
    std::vector<uint8_t> splitKey;
    BTreeLeafNodeInterpolationSearchBigNode *newRightSibling = nullptr;
    uint16_t lastEntryIndex;

    // Determine the correct splitKey and the new right sibling as well as the index of the last key that should remain
    // In the current node
    if (insertIndex < splitIndex) {
      splitKey = getFullKey(splitIndex - 1);
      newRightSibling = reinterpret_cast<BTreeLeafNodeInterpolationSearchBigNode *>(splitNode(splitIndex, splitKey));
      lastEntryIndex = splitIndex - 1;
    } else if (insertIndex > splitIndex) {
      splitKey = getFullKey(splitIndex);
      newRightSibling = reinterpret_cast<BTreeLeafNodeInterpolationSearchBigNode *>(splitNode(splitIndex + 1, splitKey));
      lastEntryIndex = splitIndex;
    } else {
      splitKey = std::vector<uint8_t>(key.begin(), key.end());
      newRightSibling = reinterpret_cast<BTreeLeafNodeInterpolationSearchBigNode *>(splitNode(splitIndex, splitKey));
      lastEntryIndex = splitIndex - 1;
    }

    // Create a new node that will replace the current node because the prefixLen could change so all keys need to be reinserted
    auto lowerFenceKey = getLowerFenceKey();
    BTreeLeafNodeInterpolationSearchBigNode *newThisChild = new BTreeLeafNodeInterpolationSearchBigNode(lowerFenceKey, splitKey);
    newThisChild->nextLeafNodeInterpolationSearchBigNode = newRightSibling;

    // Insert all the entreis remaining in the current node into the new node
    for (uint16_t i = 0; i <= lastEntryIndex; ++i) {
      auto key = getFullKey(i);
      auto value = reinterpret_cast<BTreeLeafNodeInterpolationSearchBigNode *>(this)->getValue(i);
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
    std::swap(*reinterpret_cast<BTreeLeafNodeInterpolationSearchBigNode *>(this), *newThisChild);
    delete newThisChild;
    return std::make_pair(newRightSibling, splitKey);
  }
}

bool BTreeNodeInterpolationSearchBigNode::remove(std::span<uint8_t> key) {
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
    return reinterpret_cast<BTreeInnerNodeInterpolationSearchBigNode *>(this)->getChild(childIndex)->remove(key);
  }
}
