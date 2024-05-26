#ifndef EYTZINGER_LAYOUT_FOR_B_TREE_NODES_BTREE_LINEARSEARCH_UTILS_HPP
#define EYTZINGER_LAYOUT_FOR_B_TREE_NODES_BTREE_LINEARSEARCH_UTILS_HPP

#include <bit>
#include <cstdint>
#include <cstring>
#include <span>

inline bool operator<(std::span<uint8_t> lhs, std::span<uint8_t> rhs) {
  uint16_t minLength = std::min(lhs.size(), rhs.size());
  if (minLength == 0) {
    return lhs.size() < rhs.size();
  }
  int comparisonResult = std::memcmp(lhs.data(), rhs.data(), minLength);
  if (comparisonResult == 0) {
    return lhs.size() < rhs.size();
  } else {
    return comparisonResult < 0;
  }
}

inline bool operator==(std::span<uint8_t> lhs, std::span<uint8_t> rhs) {
  if (lhs.size() == 0 && rhs.size() == 0) {
    return true;
  }
  return lhs.size() == rhs.size() && std::memcmp(lhs.data(), rhs.data(), lhs.size()) == 0;
}

inline bool operator!=(std::span<uint8_t> lhs, std::span<uint8_t> rhs) { return !(lhs == rhs); }

inline bool operator>(std::span<uint8_t> lhs, std::span<uint8_t> rhs) { return rhs < lhs; }

inline bool operator<=(std::span<uint8_t> lhs, std::span<uint8_t> rhs) { return !(lhs > rhs); }

inline bool operator>=(std::span<uint8_t> lhs, std::span<uint8_t> rhs) { return !(lhs < rhs); }

inline uint32_t convertLittleEndianToBigEndian(uint32_t littleEndianValue) { return ((littleEndianValue & 0xFF000000) >> 24) | ((littleEndianValue & 0x00FF0000) >> 8) | ((littleEndianValue & 0x0000FF00) << 8) | ((littleEndianValue & 0x000000FF) << 24); }

inline uint32_t getKeyHead(std::span<uint8_t> key) {
  uint32_t keyHead = 0;
  if (key.size() > 0) {
    std::memcpy(&keyHead, key.data(), std::min(key.size(), sizeof(uint32_t)));
    if constexpr (std::endian::native == std::endian::little) {
      keyHead = convertLittleEndianToBigEndian(keyHead);
    }
  }
  return keyHead;
}

#endif // EYTZINGER_LAYOUT_FOR_B_TREE_NODES_BTREE_LINEARSEARCH_UTILS_HPP