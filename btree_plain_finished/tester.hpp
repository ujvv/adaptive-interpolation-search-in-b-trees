#ifndef EYTZINGER_LAYOUT_FOR_B_TREE_NODES_BTREE_MINE_TESTER_HPP
#define EYTZINGER_LAYOUT_FOR_B_TREE_NODES_BTREE_MINE_TESTER_HPP

#include "btree.hpp"
#include <cassert>
#include <cstring>
#include <map>
#include <vector>
#include <cstdlib>

struct Tester {
    BTree *btree;
    std::map<std::vector<uint8_t>, std::vector<uint8_t>> stdMap;

    Tester() : btree(btree_create()), stdMap() {}

    ~Tester() { btree_destroy(btree); }

    void insert(std::vector<uint8_t> &key, std::vector<uint8_t> &value) {
#ifndef NDEBUG
        stdMap[key] = value;
#endif
        btree_insert(btree, key.data(), key.size(), value.data(), value.size());
    }

    void lookup(std::vector<uint8_t> &key) {
        uint16_t lenOut = 0;
        uint8_t *value = btree_lookup(btree, key.data(), key.size(), lenOut);
#ifdef NDEBUG
        if(lenOut!=key.size() || (lenOut>0 && value[0] != key[0]))
            throw;
#else
        auto it = stdMap.find(key);
        if (it == stdMap.end()) {
            assert(value == nullptr);
        } else {
            assert(value != nullptr);
            assert(lenOut == it->second.size());
            if (lenOut > 0)
                assert(memcmp(value, it->second.data(), lenOut) == 0);
        }
#endif
    }

    void remove(std::vector<uint8_t> &key) {
        bool wasPresentBtree = btree_remove(btree, key.data(), key.size());
#ifndef NDEBUG
        auto it = stdMap.find(key);
        bool wasPresent = it != stdMap.end();
        if (wasPresent) {
            stdMap.erase(it);
        }
        assert(wasPresent == wasPresentBtree);
#endif
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
#ifndef NDEBUG
        auto std_iterator = stdMap.lower_bound(key);
#endif
        btree_scan(
                btree, key.data(), key.size(), keyOut,
                [&](unsigned keyLen, uint8_t *payload, unsigned payloadLen) {
#ifdef NDEBUG
                    if(keyLen!=payloadLen || (payloadLen>0 && payload[0] != keyOut[0]))
                        throw;
#else
                    assert(shouldContinue);
                    assert(std_iterator != stdMap.end());
                    assert(std_iterator->first.size() == keyLen);
                    if (keyLen)
                        assert(memcmp(std_iterator->first.data(), keyOut, keyLen) == 0);
                    assert(std_iterator->second.size() == payloadLen);
                    if (payloadLen)
                        assert(memcmp(std_iterator->second.data(), payload, payloadLen) == 0);
                    ++std_iterator;
#endif
                    shouldContinue = found_record_cb(keyLen, payload, payloadLen);
                    return shouldContinue;
                });
#ifndef NDEBUG
        if (shouldContinue) {
            assert(std_iterator == stdMap.end());
        }
#endif
    }
};

#endif // DSE_PRACTICAL_COURSE_BTREE_TEMPLATE_TESTER_HPP
