#include "tester.hpp"
#include "PerfEvent.hpp"
#include <algorithm>
#include <csignal>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

void runTest(vector<vector<uint8_t>> &keys) {
    std::random_shuffle(keys.begin(), keys.end());
    Tester t{};

    std::vector<uint8_t> emptyKey{};
    uint64_t count = keys.size();

    {
        t.insert(emptyKey, keys[0]);
        t.scanThrough(emptyKey);
        for (uint64_t i = 0, j = 0; i < count; ++i, j++) {
            t.insert(keys[i], keys[i]);
            //if (i > 24000 && j >= 100) {
                t.scanThrough(emptyKey);
            //    j = 0;
            //}
        }
    }
    {
        t.scanThrough(emptyKey);
        for (uint64_t i = 0; i < count - 5; i += 5) {
            unsigned limit = 10;
            t.scan(keys[i], [&](uint16_t, uint8_t *, uint16_t) {
                limit -= 1;
                return limit > 0;
            });
        }
    }
    {
        for (uint64_t i = 0; i < count; ++i) {
            t.lookup(keys[i]);
        }
    }
    {
        for (uint64_t i = 0; i < count; ++i) {
            t.remove(keys[i]);
        }
    }
}

void runTestReverse(vector<vector<uint8_t>> &keys) {
    Tester t{};

    std::vector<uint8_t> emptyKey{};
    uint64_t count = keys.size();

    {
        for (uint64_t i = count-1; i > 0; i--) {
            t.insert(keys[i], keys[i]);
        }
    }
    {
        for (uint64_t i = 0; i < count - 5; i += 5) {
            unsigned limit = 10;
            t.scan(keys[i], [&](uint16_t, uint8_t *, uint16_t) {
                limit -= 1;
                return limit > 0;
            });
        }
    }
    {
        for (uint64_t i = count-1; i > 0; --i) {
            t.lookup(keys[i]);
        }
    }
    {
        for (uint64_t i = count-1; i > 0; --i) {
            t.remove(keys[i]);
        }
    }
    std::cout << "runTestReverse PASSED" << std::endl;
}

void runTestZickZack(vector<vector<uint8_t>> &keys) {
    Tester t{};

    std::vector<uint8_t> emptyKey{};
    uint64_t count = keys.size();

    {
        // insert i-t first and i-t last one after another
        for (uint64_t i = 0; i < count/2; ++i) {
            t.insert(keys[i], keys[i]);
            t.insert(keys[count-1-i], keys[count-1-i]);
        }
    }
    {
        for (uint64_t i = 0; i < count - 5; i += 5) {
            unsigned limit = 10;
            t.scan(keys[i], [&](uint16_t, uint8_t *, uint16_t) {
                limit -= 1;
                return limit > 0;
            });
        }
    }
    {
        for (uint64_t i = 0; i < count; ++i) {
            t.lookup(keys[i]);
        }
    }
    {
        for (uint64_t i = 0; i < count; ++i) {
            t.remove(keys[i]);
        }
    }
    std::cout << "runTestZickZack PASSED" << std::endl;
}

void runTestMixed(vector<vector<uint8_t>> &keys) {
    vector<uint8_t> firstKey = keys[0];
    std::random_shuffle(keys.begin(), keys.end());
    Tester t{};

    // Add emptyKey so scanning can always start at the beginning:
    std::vector<uint8_t> emptyKey{};
    keys[keys.size()-1] = emptyKey;
    
    uint64_t count = keys.size();
    
    // Remove Non-Existing keys
    for (uint64_t i = 0; i < (count*0.1); ++i) {
        t.remove(keys[i]);
    }

    // Lookup Non-Existing keys
    for (uint64_t i = (count*0.2); i < (count*0.3); ++i) {
        t.lookup(keys[i]);
    }
    
    // Insert firstKey and emptyKey
    t.insert(firstKey, firstKey);
    t.scanThrough(firstKey);
    t.insert(emptyKey,emptyKey);
    t.scanThrough(emptyKey);

    // Insert half of the keys
    for (uint64_t i = 0; i < (count*0.5); ++i) {
        t.insert(keys[i], keys[i]);
        //t.scanThrough(emptyKey);
    }

    // Check Tree Structure
    t.scanThrough(emptyKey);
    t.scanThrough(firstKey);

    // Double Insert
    for (uint64_t i = 0; i < (count*0.25); ++i) {
        t.insert(keys[i], keys[i]);
    }

    // Double Insert with different Values
    for (uint64_t i = (count*0.25), j = (count * 0.45); i < (count*0.5) && j >= 0; ++i, --j) {
        t.insert(keys[i], keys[j]);
    }

    // Check Tree Structure
    t.scanThrough(emptyKey);
    t.scanThrough(firstKey);

    // Remove half
    for (uint64_t i = 0; i < (count*0.25); ++i) {
        t.remove(keys[i]);
    }

    // Insert firstKey and emptyKey (making sure there are still in btree)
    t.insert(firstKey, firstKey);
    t.insert(emptyKey, emptyKey);

    // Check Tree Structure
    t.scanThrough(emptyKey);
    t.scanThrough(firstKey);

    // Reshuffle Keys
    std::random_shuffle(keys.begin(), keys.end());

    // Insert half
    for (uint64_t i = 0; i < (count*0.5); ++i) {
        t.insert(keys[i], keys[i]);
    }

    // Check Tree Structure
    t.scanThrough(emptyKey);
    t.scanThrough(firstKey);

    // Lookup some
    for (uint64_t i = 0; i < (count*0.5); i += 5) {
        t.lookup(keys[i]);
    }

    // Remove some
    for (uint64_t i = 0; i < count; i += 16) {
        t.remove(keys[i]);
    }

    // Insert firstKey and emptyKey (making sure there are still in btree)
    t.insert(firstKey, firstKey);
    t.insert(emptyKey,emptyKey);

    // Check Tree Structure
    t.scanThrough(emptyKey);
    t.scanThrough(firstKey);

    // Reshuffle Keys
    std::random_shuffle(keys.begin(), keys.end());
    
    // Insert all
    for (uint64_t i = 0; i < count; ++i) {
        t.insert(keys[i], keys[i]);
    }

    // Check Tree Structure
    t.scanThrough(emptyKey);
    t.scanThrough(firstKey);

    // Lookup some
    for (uint64_t i = 0; i < (count*0.5); i += 7) {
        t.lookup(keys[i]);
    }

    // Remove half
    for (uint64_t i = 0; i < (count*0.5); ++i) {
        t.remove(keys[i]);
    }

    // Insert firstKey and emptyKey (making sure there are still in btree)
    t.insert(firstKey, firstKey);
    t.insert(emptyKey,emptyKey);

    // Check Tree Structure
    t.scanThrough(emptyKey);
    t.scanThrough(firstKey);

    // Remove all
    for (uint64_t i = 0; i < count; ++i) {
        t.remove(keys[i]);
    }

    // Check Tree Structure
    t.scanThrough(emptyKey);
    t.scanThrough(firstKey);

    std::cout << "runTestMixed PASSED" << std::endl;
}

void runPerformanceTestStandard(vector<vector<uint8_t>> &keys,PerfEvent& perf) {
    std::random_shuffle(keys.begin(), keys.end());
    
    TesterPerformanceBinarySearch t_binarySearch{};
    TesterPerformanceTempl t_templ{};
    TesterPerformanceMap t_map;

    uint64_t count = keys.size();

    {
        PerfEventBlock peb(perf,count,{"insert BinarySearch"});
        for (uint64_t i = 0; i < count; ++i) {
            t_binarySearch.insert(keys[i], keys[i]);
        }
    }
    {
        PerfEventBlock peb(perf,count,{"insert Template"});
        for (uint64_t i = 0; i < count; ++i) {
            t_templ.insert(keys[i], keys[i]);
        }
    }
    /*{
        PerfEventBlock peb(perf,count,{"insert stdMap"});
        for (uint64_t i = 0; i < count; ++i) {
            t_map.insert(keys[i], keys[i]);
        }
    }*/
    {
        PerfEventBlock peb(perf,count,{"lookup BinarySearch"});
        for (uint64_t i = 0; i < count; ++i) {
            t_binarySearch.lookup(keys[i]);
        }
    }
    {
        PerfEventBlock peb(perf,count,{"lookup Template"});
        for (uint64_t i = 0; i < count; ++i) {
            t_templ.lookup(keys[i]);
        }
    }
    /*{
        PerfEventBlock peb(perf,count,{"lookup stdMap"});
        for (uint64_t i = 0; i < count; ++i) {
            t_map.lookup(keys[i]);
        }
    }*/
    {
        PerfEventBlock peb(perf,count,{"remove BinarySearch"});
        for (uint64_t i = 0; i < count; ++i) {
            t_binarySearch.remove(keys[i]);
        }
    }
    {
        PerfEventBlock peb(perf,count,{"remove Template"});
        for (uint64_t i = 0; i < count; ++i) {
            t_templ.remove(keys[i]);
        }
    }
    /*{
        PerfEventBlock peb(perf,count,{"remove stdMap"});
        for (uint64_t i = 0; i < count; ++i) {
            t_templ.remove(keys[i]);
        }
    }*/
}

void runPerformanceTestMixed(vector<vector<uint8_t>> &keys,PerfEvent& perf) {
    std::random_shuffle(keys.begin(), keys.end());
    
    TesterPerformanceBinarySearch t_binarySearch{};
    TesterPerformanceTempl t_templ{};

    uint64_t count = keys.size();

    vector<uint64_t> index2;
    for (uint64_t i = 0; i < count; ++i) {
        index2.push_back(i);
    }
    std::random_shuffle(index2.begin(), index2.end());

    vector<uint64_t> index3;
    for (uint64_t i = 0; i < count; ++i) {
        index3.push_back(i);
    }
    std::random_shuffle(index3.begin(), index3.end());

    vector<uint64_t> index4;
    for (uint64_t i = 0; i < count; ++i) {
        index4.push_back(i);
    }
    std::random_shuffle(index4.begin(), index4.end());

    {
        PerfEventBlock peb(perf,count,{"PerformanceTestMixed BinarySearch"});
        for (uint64_t i = 0; i < count; ++i) {
            t_binarySearch.insert(keys[i], keys[i]);
        }
        for (uint64_t i = 0; i < count; ++i) {
            t_binarySearch.lookup(keys[index2[i]]);
            t_binarySearch.remove(keys[index3[i]]);
            t_binarySearch.insert(keys[index3[i]], keys[index4[i]]);
        }
    }
    {
        PerfEventBlock peb(perf,count,{"PerformanceTestMixed Template"});
        for (uint64_t i = 0; i < count; ++i) {
            t_templ.insert(keys[i], keys[i]);
        }
        for (uint64_t i = 0; i < count; ++i) {
            t_templ.lookup(keys[index2[i]]);
            t_templ.remove(keys[index3[i]]);
            t_templ.insert(keys[index3[i]], keys[index4[i]]);
        }
    }    
}

void runPerformanceTestLookup(vector<vector<uint8_t>> &keys,PerfEvent& perf) {
    std::random_shuffle(keys.begin(), keys.end());
    
    TesterPerformanceBinarySearch t_binarySearch{};
    TesterPerformanceTempl t_templ{};

    uint64_t count = keys.size();

    vector<uint64_t> iteration2;
    for (uint64_t i = 0; i < count; ++i) {
        iteration2.push_back(i);
    }
    std::random_shuffle(iteration2.begin(), iteration2.end());

    vector<uint64_t> iteration3;
    for (uint64_t i = 0; i < count; ++i) {
        iteration3.push_back(i);
    }
    std::random_shuffle(iteration3.begin(), iteration3.end());

    // Pre-Insert all keys in different Btrees for Lookup Performance Test
    for (uint64_t i = 0; i < count; ++i) {
            t_binarySearch.insert(keys[i], keys[i]);
            t_templ.insert(keys[i], keys[i]);
        }
    
    {
        PerfEventBlock peb(perf,count,{"PerformanceTestLookup BinarySearch"});
        for (uint64_t i = 0; i < count; ++i) {
            t_binarySearch.lookup(keys[i]);
        }
        for (uint64_t i = 0; i < count; ++i) {
            t_binarySearch.lookup(keys[iteration2[i]]);
        }
        for (uint64_t i = 0; i < count; ++i) {
            t_binarySearch.lookup(keys[iteration3[i]]);
        }
    }
    {
        PerfEventBlock peb(perf,count,{"PerformanceTestLookup BinarySearch"});
        for (uint64_t i = 0; i < count; ++i) {
            t_templ.lookup(keys[i]);
        }
        for (uint64_t i = 0; i < count; ++i) {
            t_templ.lookup(keys[iteration2[i]]);
        }
        for (uint64_t i = 0; i < count; ++i) {
            t_templ.lookup(keys[iteration3[i]]);
        }
    }
}

std::vector<uint8_t> stringToVector(const std::string &str) {
    return std::vector<uint8_t>(str.begin(), str.end());
}

int main() {
    srand(42);
    PerfEvent perf;
    if (getenv("INT")) {
        vector<vector<uint8_t>> data;
        vector<uint64_t> v;
        uint64_t n = atof(getenv("INT"));
        for (uint64_t i = 0; i < n; i++)      
            v.push_back(i);
        for (auto x: v) {
            union {
                uint32_t x;
                uint8_t bytes[4];
            } u;
            u.x = x;
            data.emplace_back(u.bytes, u.bytes + 4);
        }
        //runPerformanceTestStandard(data, perf);
        //runPerformanceTestMixed(data, perf);
        //runPerformanceTestLookup(data, perf);
        runTest(data);
        runTestReverse(data);
        runTestZickZack(data);
        runTestMixed(data);
    }

    if (getenv("BYTE")) {
        vector<vector<uint8_t>> data;
        uint64_t n = atof(getenv("INT")); // Use number of entries from INT
        uint16_t byteSize = atof(getenv("BYTE"));
        
        vector<uint8_t> singleBytes;
        for (uint8_t i = 0; singleBytes.size() <= n + 1001; i++) {
            singleBytes.push_back(i);
        }
        std::random_shuffle(singleBytes.begin(), singleBytes.end());

        vector<uint64_t> randomIndex;
        for(uint64_t i = 0; i < n; i++) {
            randomIndex.push_back(i);
        }
        std::random_shuffle(randomIndex.begin(), randomIndex.end());

        for (uint64_t i = 0; i < n; i++) {
            vector<uint8_t> key;
            for (uint16_t j = 0; j < byteSize; j++) {
                key.push_back(singleBytes.at(randomIndex.at(i) + j));
            }
            data.push_back(key);
        }
        //runPerformanceTestStandard(data, perf);
        //runPerformanceTestMixed(data, perf);
        //runPerformanceTestLookup(data, perf);
        runTest(data);
        runTestReverse(data);
        runTestZickZack(data);
        runTestMixed(data);
    }

    if (getenv("VARIABLEBYTE")) {
        vector<vector<uint8_t>> data;
        uint64_t n = atof(getenv("VARIABLEBYTE"));

        uint16_t keyMaxByteSize = 1000;
        uint16_t bigKeyByteSizeIterations = 0; // Number of iterations for keys over 500 Byte to focus mostly on keys under 500 byte
        vector<uint16_t> keyByteSizes;
        for (int j = 0; j < bigKeyByteSizeIterations; j++) {
            for (uint16_t i = 0; i <= keyMaxByteSize && i < n; i++) {
                keyByteSizes.push_back(i);
            }
        }
        while(keyByteSizes.size() < n) {
            for (uint16_t i = 0; i <= 500 && keyByteSizes.size() < n; i++) {
                keyByteSizes.push_back(i);
            }
        }
        vector<uint8_t> singleBytes;
        for (uint8_t i = 0; singleBytes.size() <= keyByteSizes.size() + 1001; i++) {
            singleBytes.push_back(i);
        }
        std::random_shuffle(singleBytes.begin(), singleBytes.end());

        vector<uint64_t> randomIndex;
        for(uint64_t i = 0; i < keyByteSizes.size(); i++) {
            randomIndex.push_back(i);
        }
        std::random_shuffle(randomIndex.begin(), randomIndex.end());

        for (uint64_t i = 0; i < keyByteSizes.size(); i++) {
            vector<uint8_t> key;
            for (uint16_t j = 0; j < keyByteSizes.at(i); j++) {
                key.push_back(singleBytes.at(randomIndex.at(i) + j));
            }
            data.push_back(key);
        }
        runTest(data);
        runTestReverse(data);
        runTestZickZack(data);
        runTestMixed(data);
    }

    if (getenv("LONG1")) {
        vector<vector<uint8_t>> data;
        uint64_t n = atof(getenv("LONG1"));
        for (unsigned i = 0; i < n; i++) {
            string s;
            for (unsigned j = 0; j < i; j++)
                s.push_back('A');
            data.push_back(stringToVector(s));;
        }
        //runPerformanceTestStandard(data, perf);
        //runPerformanceTestMixed(data, perf);
        //runPerformanceTestLookup(data, perf);
        runTest(data);
        runTestReverse(data);
        runTestZickZack(data);
        runTestMixed(data);
    }

    if (getenv("LONG2")) {
        vector<vector<uint8_t>> data;
        uint64_t n = atof(getenv("LONG2"));
        for (unsigned i = 0; i < n; i++) {
            string s;
            for (unsigned j = 0; j < i; j++)
                s.push_back('A' + random() % 60);
            data.push_back(stringToVector(s));
        }
        //runPerformanceTestStandard(data, perf);
        //runPerformanceTestMixed(data, perf);
        //runPerformanceTestLookup(data, perf);
        runTest(data);
        runTestMixed(data);
    }

    if (getenv("FILE")) {
        vector<vector<uint8_t>> data;
        ifstream in(getenv("FILE"));
        string line;
        while (getline(in, line))
            data.push_back(stringToVector(line));;
        
        //runPerformanceTestStandard(data, perf);
        //runPerformanceTestMixed(data, perf);
        //runPerformanceTestLookup(data, perf);
        runTest(data);
        runTestMixed(data);
    }
    return 0;
}