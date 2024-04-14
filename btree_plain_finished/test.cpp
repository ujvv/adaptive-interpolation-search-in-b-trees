/*#include "btree.hpp"

#include <fstream>
#include <iostream>
#include <map>
#include <random>
#include <string>

#include "tester.hpp"

std::vector<uint8_t> stringToVector(const std::string &str) { return std::vector<uint8_t>(str.begin(), str.end()); }

bool checkTreeStructure(BTreeNode *root) {
  if (root == nullptr) {
    std::cout << "Node is null" << std::endl;
    return false;
  }

  // Check if keys are sorted
  auto keys = root->getKeys();
  if (!std::is_sorted(keys.begin(), keys.end())) {
    std::cout << "Keys are not sorted" << std::endl;
    return false;
  }

  if (root->isLeaf) {
    return true;
  }

  // Check if any childs have a key greater then the parent key
  for (int i = 0; i < root->numKeys; ++i) {
    auto key = root->getFullKey(i);
    auto childKeys = reinterpret_cast<BTreeInnerNode *>(root)->getChild(i)->getKeys();
    if (childKeys.size() == 0) {
      continue;
    }
    auto max_key = *max_element(childKeys.begin(), childKeys.end());
    if (max_key > key) {
      std::cout << "Child key is greater than parent key" << std::endl;
      return false;
    }
  }

  // Check if all children have a valid structure
  for (int i = 0; i <= root->numKeys; ++i) {
    if (!checkTreeStructure(reinterpret_cast<BTreeInnerNode *>(root)->getChild(i))) {
      return false;
    }
  }

  return true;
}

void runTest(std::vector<std::vector<uint8_t>> &keys, std::vector<std::vector<uint8_t>> &values, bool checkStructure, BTree &tree, bool print = false) {
  std::map<std::vector<uint8_t>, std::vector<uint8_t>> expectedState;

  // Insert keys and values
  if (print) {
    std::cout << "Inserting keys" << std::endl;
  }
  for (uint32_t i = 0; i < keys.size(); ++i) {
    if (print) {
      std::cout << "Inserting key " << i << std::endl;
    }
    tree.insert(keys[i], values[i]);
    expectedState[keys[i]] = values[i];

    // Check for valid tree structure
    if (checkStructure && !checkTreeStructure(tree.root)) {
      std::cout << "Tree structure is invalid" << std::endl;
      tree.destroy();
      exit(1);
    }
  }

  // Check for valid tree structure
  if (!checkTreeStructure(tree.root)) {
    std::cout << "Tree structure is invalid" << std::endl;
    tree.destroy();
    exit(1);
  }

  // Check if all keys are in the tree
  if (print) {
    std::cout << "Looking up keys" << std::endl;
  }
  for (uint32_t i = 0; i < keys.size(); ++i) {
    auto actualValue = tree.lookup(keys[i]);
    auto expectedValue = expectedState[keys[i]];
    if (!actualValue.has_value() || actualValue->size() != expectedValue.size() || !std::equal(actualValue->begin(), actualValue->end(), expectedValue.begin())) {
      std::cout << "Key " << i << " was not found" << std::endl;
      tree.destroy();
      exit(1);
    }
  }

  // Remove all keys
  if (print) {
    std::cout << "Removing keys" << std::endl;
  }
  for (uint32_t i = 0; i < keys.size(); ++i) {
    if (print) {
      std::cout << "Removing key " << i << std::endl;
    }
    // We can't remove the same key twice
    if (!expectedState.count(keys[i])) {
      continue;
    }

    bool success = tree.remove(keys[i]);
    expectedState.erase(keys[i]);
    if (!success) {
      std::cout << "Key " << i << " could not be removed" << std::endl;
      tree.destroy();
      exit(1);
    }

    // Check for valid tree structure
    if (checkStructure && !checkTreeStructure(tree.root)) {
      std::cout << "Tree structure is invalid" << std::endl;
      tree.destroy();
      exit(1);
    }
  }

  // Check if all keys are removed
  if (print) {
    std::cout << "Looking up keys" << std::endl;
  }
  for (uint32_t i = 0; i < keys.size(); ++i) {
    auto value = tree.lookup(keys[i]);
    if (value.has_value()) {
      std::cout << "Key " << i << " was not removed" << std::endl;
      tree.destroy();
      exit(1);
    }
  }

  // Check tree structure
  if (!checkTreeStructure(tree.root)) {
    std::cout << "Tree structure is invalid" << std::endl;
    tree.destroy();
    exit(1);
  }

  // Check if expectedState map is empty
  if (!expectedState.empty()) {
    std::cout << "Expected state is not empty" << std::endl;
    tree.destroy();
    exit(1);
  }
}

std::vector<std::vector<uint8_t>> generateRandomByteArrays(uint16_t maxLength, uint32_t amount) {
  std::mt19937 gen(42);
  std::uniform_int_distribution<uint8_t> valueDistribution(0, 255);
  std::uniform_int_distribution<int> lengthDistribution(0, maxLength);
  std::vector<std::vector<uint8_t>> result;

  for (uint32_t i = 0; i < amount; ++i) {
    uint32_t length = lengthDistribution(gen);
    std::vector<uint8_t> key(length);
    std::generate(key.begin(), key.end(), [&]() { return valueDistribution(gen); });
    result.push_back(key);
  }

  return result;
}

void runRandomTest() {
  std::cout << "Running random test" << std::endl;
  BTree tree;
  uint32_t targetAmount = 50000;
  uint8_t iterations = 5;
  bool checkStructure = false;

  for (uint8_t i = 0; i < iterations; ++i) {
    std::vector<std::vector<uint8_t>> keys = generateRandomByteArrays(400, targetAmount);
    std::vector<std::vector<uint8_t>> values = generateRandomByteArrays(400, targetAmount);
    runTest(keys, values, checkStructure, tree);
    std::cout << "Iteration " << (int)i << " passed" << std::endl;
  }

  tree.destroy();
  std::cout << "Random test passed" << std::endl;
}

void runUrlTest() {
  std::cout << "Running url test" << std::endl;
  std::vector<std::vector<uint8_t>> data;
  std::ifstream in("../data/urls");
  std::string line;
  uint32_t amount = 100000;

  while (getline(in, line)) {
    if (data.size() >= amount) {
      break;
    }
    data.push_back(stringToVector(line));
  }
  BTree tree;
  runTest(data, data, false, tree, true);
  tree.destroy();
  std::cout << "Url test passed" << std::endl;
}

void runLong1Test(bool noScan, uint64_t n) {
  std::vector<std::vector<uint8_t>> keys;
  for (unsigned i = 0; i < n; i++) {
    std::string s;
    for (unsigned j = 0; j < i; j++)
      s.push_back('A');
    keys.push_back(stringToVector(s));
  }

  std::random_shuffle(keys.begin(), keys.end());
  uint64_t count = keys.size();
  Tester tester{};

  // Insert keys
  for (uint64_t i = 0; i < count; ++i) {
    tester.insert(keys[i], keys[i]);
  }

  // Check for valid tree structure
  assert (checkTreeStructure(tester.btree->root));

  if (!noScan) {
    // Scan
    for (uint64_t i = 0; i < count; i += 5) {
      unsigned limit = 10;
      tester.scan(keys[i], [&](uint16_t, uint8_t *, uint16_t) {
        limit -= 1;
        return limit > 0;
      });
    }
  }

  // Lookup
  for (uint64_t i = 0; i < count; ++i) {
    std::cout << i << std::endl;
    tester.lookup(keys[i]);
  }

  // Remove
  for (uint64_t i = 0; i < count; ++i) {
    tester.remove(keys[i]);
  }

  std::cout << "Long1 test passed" << std::endl;
}

int main() {
  runRandomTest();
  runUrlTest();
  runLong1Test(false, 480);
  return 0;
} */