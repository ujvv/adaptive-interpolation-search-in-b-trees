#ifndef EYTZINGER_LAYOUT_FOR_B_TREE_NODES_TESTER_HPP
#define EYTZINGER_LAYOUT_FOR_B_TREE_NODES_TESTER_HPP

#include "btree_template/btree_template.hpp"
#include "btree_template/btreenode_template.hpp"
//#include "btree_template_bigNode/btree_template.hpp"
//#include "btree_template_bigNode/btreenode_template.hpp"

#include "btree_binarySearch/btree_binarySearch.hpp"
#include "btree_binarySearch/btreenode_binarySearch.hpp"
#include "btree_binarySearch_withHints/btree_binarySearchHints.hpp"
#include "btree_binarySearch_withHints/btreenode_binarySearchHints.hpp"

#include "btree_linearSearch/btree_linearSearch.hpp"
#include "btree_linearSearch/btreenode_linearSearch.hpp"

#include "btree_interpolationSearch/btree_interpolationSearch.hpp"
#include "btree_interpolationSearch/btreenode_interpolationSearch.hpp"

#include "btree_plain_finished/btree.hpp"

#include <cassert>
#include <cstring>
#include <map>
#include <vector>
#include <cstdlib>
#include <iostream>

struct Tester {
    BTreeLinearSearch *btree;

    BTreeTemplate *btreeTemplate;

    std::map<std::vector<uint8_t>, std::vector<uint8_t>> stdMap;

    Tester() : btree(btree_create_linearSearch()), btreeTemplate(btree_create_template()), stdMap()  {}

    ~Tester() { btree_destroy_linearSearch(btree); btree_destroy_template(btreeTemplate); }

    void insert(std::vector<uint8_t> &key, std::vector<uint8_t> &value) {
        stdMap[key] = value;
        
        btree_insert_linearSearch(btree, key.data(), key.size(), value.data(), value.size());
        btree_insert_template(btreeTemplate, key.data(), key.size(), value.data(), value.size());
    }

    void lookup(std::vector<uint8_t> &key) {
        stdMap.find(key);
        
        uint16_t lenOut = 0;
        uint16_t lenOutTemplate = 0;
        uint8_t *value = btree_lookup_linearSearch(btree, key.data(), key.size(), lenOut);
        uint8_t *valueTemplate = btree_lookup_template(btreeTemplate, key.data(), key.size(), lenOutTemplate);
        std::span<uint8_t> spanTemplate = {valueTemplate, lenOutTemplate};

        if (valueTemplate == nullptr) {
            assert(value == nullptr);
        } else {
            if (value == nullptr) {
                std::cout << "valueTemplate: " <<  std::string(spanTemplate.begin(), spanTemplate.end()) <<" lenOutTemplate: " << lenOutTemplate << " lenOut: " << lenOut << std::endl;
            }
            assert(value != nullptr);
            assert(lenOutTemplate == lenOut);
            if (lenOut > 0)
                assert(memcmp(value, valueTemplate, lenOut) == 0);
        }
    }

    void remove(std::vector<uint8_t> &key) {
            auto it = stdMap.find(key);
            bool wasPresent = it != stdMap.end();
            if (wasPresent) {
                stdMap.erase(it);
            }

        bool wasPresentBtreeTemplate = btree_remove_template(btreeTemplate, key.data(), key.size());
        bool wasPresentBtree = btree_remove_linearSearch(btree, key.data(), key.size());
        assert(wasPresentBtreeTemplate == wasPresentBtree);

        (void)wasPresentBtree;
    }
 
    void compare_btrees(std::vector<uint8_t> &key) {
        /* 
         if (btreeTemplate->root == nullptr) {
                assert(btree->root == nullptr);
                return;
        }

        BTreeLeafNodeTemplate *currentNodeTemplate = btreeTemplate->traverseToLeaf_template(key);
        BTreeLeafNode *currentNode = btree->traverseToLeaf(key);

        if (currentNodeTemplate == nullptr) {
            assert(currentNode == nullptr);
            return;
        } else {
            assert(currentNode != nullptr);
        }

        std::vector<uint8_t> keyVectorTemplate(key.begin(), key.end());
        std::vector<uint8_t> keyVector(key.begin(), key.end());

        bool currentKeyValidTemplate = false;
        bool currentKeyValid = false;

        int counter = 0;
        while (currentNodeTemplate != nullptr) {
            std::vector<std::vector<uint8_t>> keysTemplate = currentNodeTemplate->getKeys();
            std::vector<std::vector<uint8_t>> keys = currentNode->getKeys();
            assert(keysTemplate.size() == keys.size());

            for (uint16_t i = 0; i < keysTemplate.size(); i++) {
                if (!currentKeyValidTemplate && keysTemplate[i] >= keyVectorTemplate) {
                    currentKeyValidTemplate = true;
                }

                if (currentKeyValidTemplate) {
                    std::span<uint8_t> valueSpanTemplate = currentNodeTemplate->getValue(i);
                    std::span<uint8_t> valueSpan = currentNode->getValue(i);

                    assert(keysTemplate[i].size() == keys[i].size());

                    if (keysTemplate[i].size() > 0) {
                        assert(memcmp(keysTemplate[i].data(), keys[i].data(), keysTemplate[i].size()) == 0);
                    }
                    
                    if (keysTemplate[i].size()==valueSpanTemplate.size()) {
                        assert(keys[i].size()==valueSpan.size());
                    }
                    
                    assert(valueSpanTemplate.size() == valueSpan.size());
                    if (valueSpanTemplate.size() > 0) {
                        assert(memcmp(valueSpanTemplate.data(), valueSpan.data(), valueSpanTemplate.size()) == 0);
                    }
                }
            }
            currentNodeTemplate = currentNodeTemplate->nextLeafNodeTemplate;
            currentNode = currentNode->nextLeafNode;
            if (currentNodeTemplate == nullptr) {
                assert(currentNode == nullptr);
            } else {
                assert(currentNode != nullptr);
            }
        }
*/
    }

    void scan(std::vector<uint8_t> &key,
              const std::function<bool(uint16_t, uint8_t *, uint16_t)>
              &found_record_cb) {

        if (getenv("NO_SCAN")) {
            return;
        }
        uint8_t keyOut[1 << 10];
        bool shouldContinue = true;

        auto std_iterator = stdMap.lower_bound(key);

        btree_scan_linearSearch(
                btree, key.data(), key.size(), keyOut,
                [&](unsigned keyLen, uint8_t *payload, unsigned payloadLen) {
 
                    if(keyLen!=payloadLen || (payloadLen>0 && payload[0] != keyOut[0]))
                        throw;

                    assert(shouldContinue);
                    assert(std_iterator != stdMap.end());
                    assert(std_iterator->first.size() == keyLen);
                    if (keyLen)
                        assert(memcmp(std_iterator->first.data(), keyOut, keyLen) == 0);
                    assert(std_iterator->second.size() == payloadLen);
                    if (payloadLen)
                        assert(memcmp(std_iterator->second.data(), payload, payloadLen) == 0);
                    ++std_iterator;

                    shouldContinue = found_record_cb(keyLen, payload, payloadLen);
                    return shouldContinue;
                });

        if (shouldContinue) {
            assert(std_iterator == stdMap.end());
        }

    }

    void scanThrough(std::vector<uint8_t> &key) {

        if (getenv("NO_SCAN")) {
            return;
        }
        uint8_t keyOut[1 << 10];
        bool shouldContinue = true;

        auto std_iterator = stdMap.lower_bound(key);

        btree_scan_linearSearch(
                btree, key.data(), key.size(), keyOut,
                [&](unsigned keyLen, uint8_t *payload, unsigned payloadLen) {
                    assert(shouldContinue);
                    assert(std_iterator != stdMap.end());
                    if(std_iterator->first.size() != keyLen) {
                        std::cout << "EXCEPTION: std_iterator->first.size() != keyLen" << std::endl;
                        std::cout << "std_iterator->first.size(): " <<  std_iterator->first.size() <<" keyLen: " << keyLen << " payloadLen: " << payloadLen << std::endl;
                    }
                    assert(std_iterator->first.size() == keyLen);
                    if (keyLen)
                        assert(memcmp(std_iterator->first.data(), keyOut, keyLen) == 0);
                    assert(std_iterator->second.size() == payloadLen);
                    if (payloadLen)
                        assert(memcmp(std_iterator->second.data(), payload, payloadLen) == 0);
                    ++std_iterator;

                    return shouldContinue;
                });

        if (shouldContinue) {
            assert(std_iterator == stdMap.end());
        }

    }
};

struct TesterPerformance {
    BTree *btree;

    TesterPerformance() : btree(btree_create()) {}

    ~TesterPerformance() { btree_destroy(btree); }

    void insert(std::vector<uint8_t> &key, std::vector<uint8_t> &value) {
        btree_insert(btree, key.data(), key.size(), value.data(), value.size());
    }

    void lookup(std::vector<uint8_t> &key) {
        uint16_t lenOut = 0;
        btree_lookup(btree, key.data(), key.size(), lenOut);
    }

    void remove(std::vector<uint8_t> &key) {
        btree_remove(btree, key.data(), key.size());
    }
};

struct TesterPerformanceTempl {

    BTreeTemplate *btreeTemplate;

    TesterPerformanceTempl() : btreeTemplate(btree_create_template())  {}

    ~TesterPerformanceTempl() { btree_destroy_template(btreeTemplate); }

    void insert(std::vector<uint8_t> &key, std::vector<uint8_t> &value) {
        btree_insert_template(btreeTemplate, key.data(), key.size(), value.data(), value.size());
    }

    void lookup(std::vector<uint8_t> &key) {
        uint16_t lenOutTemplate = 0;
        btree_lookup_template(btreeTemplate, key.data(), key.size(), lenOutTemplate);
    }

    void remove(std::vector<uint8_t> &key) {
            btree_remove_template(btreeTemplate, key.data(), key.size());
    }
};

struct TesterPerformanceBinarySearch {

    BTreeBinarySearch *btreeBinarySearch;

    TesterPerformanceBinarySearch() : btreeBinarySearch(btree_create_binarySearch())  {}

    ~TesterPerformanceBinarySearch() { btree_destroy_binarySearch(btreeBinarySearch); }

    void insert(std::vector<uint8_t> &key, std::vector<uint8_t> &value) {
        btree_insert_binarySearch(btreeBinarySearch, key.data(), key.size(), value.data(), value.size());
    }

    void lookup(std::vector<uint8_t> &key) {
        uint16_t lenOutBinarySearch = 0;
        btree_lookup_binarySearch(btreeBinarySearch, key.data(), key.size(), lenOutBinarySearch);
    }

    void remove(std::vector<uint8_t> &key) {
            btree_remove_binarySearch(btreeBinarySearch, key.data(), key.size());
    }
};

struct TesterPerformanceBinarySearchHints {

    BTreeBinarySearchHints *btreeBinarySearchHints;

    TesterPerformanceBinarySearchHints() : btreeBinarySearchHints(btree_create_binarySearchHints())  {}

    ~TesterPerformanceBinarySearchHints() { btree_destroy_binarySearchHints(btreeBinarySearchHints); }

    void insert(std::vector<uint8_t> &key, std::vector<uint8_t> &value) {
        btree_insert_binarySearchHints(btreeBinarySearchHints, key.data(), key.size(), value.data(), value.size());
    }

    void lookup(std::vector<uint8_t> &key) {
        uint16_t lenOutBinarySearchHints = 0;
        btree_lookup_binarySearchHints(btreeBinarySearchHints, key.data(), key.size(), lenOutBinarySearchHints);
    }

    void remove(std::vector<uint8_t> &key) {
            btree_remove_binarySearchHints(btreeBinarySearchHints, key.data(), key.size());
    }
};

struct TesterPerformanceLinearSearch {

    BTreeLinearSearch *btreeLinearSearch;

    TesterPerformanceLinearSearch() : btreeLinearSearch(btree_create_linearSearch())  {}

    ~TesterPerformanceLinearSearch() { btree_destroy_linearSearch(btreeLinearSearch); }

    void insert(std::vector<uint8_t> &key, std::vector<uint8_t> &value) {
        btree_insert_linearSearch(btreeLinearSearch, key.data(), key.size(), value.data(), value.size());
    }

    void lookup(std::vector<uint8_t> &key) {
        uint16_t lenOutLinearSearch = 0;
        btree_lookup_linearSearch(btreeLinearSearch, key.data(), key.size(), lenOutLinearSearch);
    }

    void remove(std::vector<uint8_t> &key) {
            btree_remove_linearSearch(btreeLinearSearch, key.data(), key.size());
    }
};

struct TesterPerformanceInterpolationSearch {

    BTreeInterpolationSearch *btreeInterpolationSearch;

    TesterPerformanceInterpolationSearch() : btreeInterpolationSearch(btree_create_interpolationSearch())  {}

    ~TesterPerformanceInterpolationSearch() { btree_destroy_interpolationSearch(btreeInterpolationSearch); }

    void insert(std::vector<uint8_t> &key, std::vector<uint8_t> &value) {
        btree_insert_interpolationSearch(btreeInterpolationSearch, key.data(), key.size(), value.data(), value.size());
    }

    void lookup(std::vector<uint8_t> &key) {
        uint16_t lenOutInterpolationSearch = 0;
        btree_lookup_interpolationSearch(btreeInterpolationSearch, key.data(), key.size(), lenOutInterpolationSearch);
    }

    void remove(std::vector<uint8_t> &key) {
            btree_remove_interpolationSearch(btreeInterpolationSearch, key.data(), key.size());
    }
};

struct TesterPerformanceMap {
    std::map<std::vector<uint8_t>, std::vector<uint8_t>> stdMap;

    TesterPerformanceMap() : stdMap() {}

    void insert(std::vector<uint8_t> &key, std::vector<uint8_t> &value) {
        stdMap[key] = value;
    }

    void lookup(std::vector<uint8_t> &key) {
        stdMap.find(key);
    }

    void remove(std::vector<uint8_t> &key) {
        auto it = stdMap.find(key);
            bool wasPresent = it != stdMap.end();
            if (wasPresent) {
                stdMap.erase(it);
            }

    }
};

#endif // EYTZINGER_LAYOUT_FOR_B_TREE_NODES_TESTER_HPP
