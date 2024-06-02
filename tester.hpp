#ifndef EYTZINGER_LAYOUT_FOR_B_TREE_NODES_TESTER_HPP
#define EYTZINGER_LAYOUT_FOR_B_TREE_NODES_TESTER_HPP

#include "btree_template/btree_template.hpp"
#include "btree_template/btreenode_template.hpp"
#include "btree_template_bigNode/btree_templateBigNode.hpp"
#include "btree_template_bigNode/btreenode_templateBigNode.hpp"


#include "btree_binarySearch_noPrefix/btree_binarySearchNoPrefix.hpp"
#include "btree_binarySearch_noPrefix/btreenode_binarySearchNoPrefix.hpp"

#include "btree_binarySearch/btree_binarySearch.hpp"
#include "btree_binarySearch/btreenode_binarySearch.hpp"
#include "btree_binarySearch_bigNode/btree_binarySearchBigNode.hpp"
#include "btree_binarySearch_bigNode/btreenode_binarySearchBigNode.hpp"

#include "btree_binarySearch_withHints/btree_binarySearchHints.hpp"
#include "btree_binarySearch_withHints/btreenode_binarySearchHints.hpp"


#include "btree_linearSearch/btree_linearSearch.hpp"
#include "btree_linearSearch/btreenode_linearSearch.hpp"
#include "btree_linearSearch_bigNode/btree_linearSearchBigNode.hpp"
#include "btree_linearSearch_bigNode/btreenode_linearSearchBigNode.hpp"

#include "btree_interpolationSearch/btree_interpolationSearch.hpp"
#include "btree_interpolationSearch/btreenode_interpolationSearch.hpp"
#include "btree_interpolationSearch_bigNode/btree_interpolationSearchBigNode.hpp"
#include "btree_interpolationSearch_bigNode/btreenode_interpolationSearchBigNode.hpp"

#include "btree_interpolationSearch_withKeyHeads/btree_interpolationSearchKeyHeads.hpp"
#include "btree_interpolationSearch_withKeyHeads/btreenode_interpolationSearchKeyHeads.hpp"
#include "btree_interpolationSearch_withKeyHeads_bigNode/btree_interpolationSearchKeyHeadsBigNode.hpp"
#include "btree_interpolationSearch_withKeyHeads_bigNode/btreenode_interpolationSearchKeyHeadsBigNode.hpp"

#include "btree_plain_finished/btree.hpp"

#include <cassert>
#include <cstring>
#include <map>
#include <vector>
#include <cstdlib>
#include <iostream>

struct Tester {
    BTreeBinarySearchNoPrefix *btree;

    BTreeTemplate *btreeTemplate;

    std::map<std::vector<uint8_t>, std::vector<uint8_t>> stdMap;

    Tester() : btree(btree_create_binarySearchNoPrefix()), btreeTemplate(btree_create_template()), stdMap()  {}

    ~Tester() { btree_destroy_binarySearchNoPrefix(btree); btree_destroy_template(btreeTemplate); }

    void insert(std::vector<uint8_t> &key, std::vector<uint8_t> &value) {
        stdMap[key] = value;
        
        btree_insert_binarySearchNoPrefix(btree, key.data(), key.size(), value.data(), value.size());
        btree_insert_template(btreeTemplate, key.data(), key.size(), value.data(), value.size());
    }

    void lookup(std::vector<uint8_t> &key) {
        stdMap.find(key);
        
        uint16_t lenOut = 0;
        uint16_t lenOutTemplate = 0;
        uint8_t *value = btree_lookup_binarySearchNoPrefix(btree, key.data(), key.size(), lenOut);
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
        bool wasPresentBtree = btree_remove_binarySearchNoPrefix(btree, key.data(), key.size());
        assert(wasPresentBtreeTemplate == wasPresentBtree);

        (void)wasPresentBtree;
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

        btree_scan_binarySearchNoPrefix(
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

        btree_scan_binarySearchNoPrefix(
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

struct BtreeAnalysis {
    BTreeBinarySearch *btree;

    BtreeAnalysis() : btree(btree_create_binarySearch()) {}

    ~BtreeAnalysis() { btree_destroy_binarySearch(btree); }

    void insert(std::vector<uint8_t> &key, std::vector<uint8_t> &value) {
        btree_insert_binarySearch(btree, key.data(), key.size(), value.data(), value.size());
    }
    
    std::vector<double> analyzeLeafs() {
        return btree_analyzeLeafs(btree);
    }

    std::vector<double> analyzeInnerNodes() {
        return btree_analyzeInnerNodes(btree);
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

struct TesterPerformanceTemplBigNode {

    BTreeTemplateBigNode *btreeTemplateBigNode;

    TesterPerformanceTemplBigNode() : btreeTemplateBigNode(btree_create_templateBigNode())  {}

    ~TesterPerformanceTemplBigNode() { btree_destroy_templateBigNode(btreeTemplateBigNode); }

    void insert(std::vector<uint8_t> &key, std::vector<uint8_t> &value) {
        btree_insert_templateBigNode(btreeTemplateBigNode, key.data(), key.size(), value.data(), value.size());
    }

    void lookup(std::vector<uint8_t> &key) {
        uint16_t lenOutTemplateBigNode = 0;
        btree_lookup_templateBigNode(btreeTemplateBigNode, key.data(), key.size(), lenOutTemplateBigNode);
    }

    void remove(std::vector<uint8_t> &key) {
            btree_remove_templateBigNode(btreeTemplateBigNode, key.data(), key.size());
    }
};

struct TesterPerformanceBinarySearchNoPrefix {

    BTreeBinarySearchNoPrefix *btreeBinarySearchNoPrefix;

    TesterPerformanceBinarySearchNoPrefix() : btreeBinarySearchNoPrefix(btree_create_binarySearchNoPrefix())  {}

    ~TesterPerformanceBinarySearchNoPrefix() { btree_destroy_binarySearchNoPrefix(btreeBinarySearchNoPrefix); }

    void insert(std::vector<uint8_t> &key, std::vector<uint8_t> &value) {
        btree_insert_binarySearchNoPrefix(btreeBinarySearchNoPrefix, key.data(), key.size(), value.data(), value.size());
    }

    void lookup(std::vector<uint8_t> &key) {
        uint16_t lenOutBinarySearch = 0;
        btree_lookup_binarySearchNoPrefix(btreeBinarySearchNoPrefix, key.data(), key.size(), lenOutBinarySearch);
    }

    void remove(std::vector<uint8_t> &key) {
            btree_remove_binarySearchNoPrefix(btreeBinarySearchNoPrefix, key.data(), key.size());
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

struct TesterPerformanceBinarySearchBigNode {

    BTreeBinarySearchBigNode *btreeBinarySearchBigNode;

    TesterPerformanceBinarySearchBigNode() : btreeBinarySearchBigNode(btree_create_binarySearchBigNode())  {}

    ~TesterPerformanceBinarySearchBigNode() { btree_destroy_binarySearchBigNode(btreeBinarySearchBigNode); }

    void insert(std::vector<uint8_t> &key, std::vector<uint8_t> &value) {
        btree_insert_binarySearchBigNode(btreeBinarySearchBigNode, key.data(), key.size(), value.data(), value.size());
    }

    void lookup(std::vector<uint8_t> &key) {
        uint16_t lenOutBinarySearch = 0;
        btree_lookup_binarySearchBigNode(btreeBinarySearchBigNode, key.data(), key.size(), lenOutBinarySearch);
    }

    void remove(std::vector<uint8_t> &key) {
            btree_remove_binarySearchBigNode(btreeBinarySearchBigNode, key.data(), key.size());
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

struct TesterPerformanceLinearSearchBigNode {

    BTreeLinearSearchBigNode *btreeLinearSearchBigNode;

    TesterPerformanceLinearSearchBigNode() : btreeLinearSearchBigNode(btree_create_linearSearchBigNode())  {}

    ~TesterPerformanceLinearSearchBigNode() { btree_destroy_linearSearchBigNode(btreeLinearSearchBigNode); }

    void insert(std::vector<uint8_t> &key, std::vector<uint8_t> &value) {
        btree_insert_linearSearchBigNode(btreeLinearSearchBigNode, key.data(), key.size(), value.data(), value.size());
    }

    void lookup(std::vector<uint8_t> &key) {
        uint16_t lenOutLinearSearchBigNode = 0;
        btree_lookup_linearSearchBigNode(btreeLinearSearchBigNode, key.data(), key.size(), lenOutLinearSearchBigNode);
    }

    void remove(std::vector<uint8_t> &key) {
            btree_remove_linearSearchBigNode(btreeLinearSearchBigNode, key.data(), key.size());
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

struct TesterPerformanceInterpolationSearchBigNode {

    BTreeInterpolationSearchBigNode *btreeInterpolationSearchBigNode;

    TesterPerformanceInterpolationSearchBigNode() : btreeInterpolationSearchBigNode(btree_create_interpolationSearchBigNode())  {}

    ~TesterPerformanceInterpolationSearchBigNode() { btree_destroy_interpolationSearchBigNode(btreeInterpolationSearchBigNode); }

    void insert(std::vector<uint8_t> &key, std::vector<uint8_t> &value) {
        btree_insert_interpolationSearchBigNode(btreeInterpolationSearchBigNode, key.data(), key.size(), value.data(), value.size());
    }

    void lookup(std::vector<uint8_t> &key) {
        uint16_t lenOutInterpolationSearchBigNode = 0;
        btree_lookup_interpolationSearchBigNode(btreeInterpolationSearchBigNode, key.data(), key.size(), lenOutInterpolationSearchBigNode);
    }

    void remove(std::vector<uint8_t> &key) {
            btree_remove_interpolationSearchBigNode(btreeInterpolationSearchBigNode, key.data(), key.size());
    }
};

struct TesterPerformanceInterpolationSearchKeyHeads {

    BTreeInterpolationSearchKeyHeads *btreeInterpolationSearchKeyHeads;

    TesterPerformanceInterpolationSearchKeyHeads() : btreeInterpolationSearchKeyHeads(btree_create_interpolationSearchKeyHeads())  {}

    ~TesterPerformanceInterpolationSearchKeyHeads() { btree_destroy_interpolationSearchKeyHeads(btreeInterpolationSearchKeyHeads); }

    void insert(std::vector<uint8_t> &key, std::vector<uint8_t> &value) {
        btree_insert_interpolationSearchKeyHeads(btreeInterpolationSearchKeyHeads, key.data(), key.size(), value.data(), value.size());
    }

    void lookup(std::vector<uint8_t> &key) {
        uint16_t lenOutInterpolationSearchKeyHeads = 0;
        btree_lookup_interpolationSearchKeyHeads(btreeInterpolationSearchKeyHeads, key.data(), key.size(), lenOutInterpolationSearchKeyHeads);
    }

    void remove(std::vector<uint8_t> &key) {
            btree_remove_interpolationSearchKeyHeads(btreeInterpolationSearchKeyHeads, key.data(), key.size());
    }
};

struct TesterPerformanceInterpolationSearchKeyHeadsBigNode {

    BTreeInterpolationSearchKeyHeadsBigNode *btreeInterpolationSearchKeyHeadsBigNode;

    TesterPerformanceInterpolationSearchKeyHeadsBigNode() : btreeInterpolationSearchKeyHeadsBigNode(btree_create_interpolationSearchKeyHeadsBigNode())  {}

    ~TesterPerformanceInterpolationSearchKeyHeadsBigNode() { btree_destroy_interpolationSearchKeyHeadsBigNode(btreeInterpolationSearchKeyHeadsBigNode); }

    void insert(std::vector<uint8_t> &key, std::vector<uint8_t> &value) {
        btree_insert_interpolationSearchKeyHeadsBigNode(btreeInterpolationSearchKeyHeadsBigNode, key.data(), key.size(), value.data(), value.size());
    }

    void lookup(std::vector<uint8_t> &key) {
        uint16_t lenOutInterpolationSearchKeyHeadsBigNode = 0;
        btree_lookup_interpolationSearchKeyHeadsBigNode(btreeInterpolationSearchKeyHeadsBigNode, key.data(), key.size(), lenOutInterpolationSearchKeyHeadsBigNode);
    }

    void remove(std::vector<uint8_t> &key) {
            btree_remove_interpolationSearchKeyHeadsBigNode(btreeInterpolationSearchKeyHeadsBigNode, key.data(), key.size());
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
