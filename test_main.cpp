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
        for (uint64_t i = 0; i < count; ++i) {
            t.insert(keys[i], keys[i]);
            //if (i >= 28700) {
            //    t.scanThrough(emptyKey);
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

void runTestMixed(vector<vector<uint8_t>> &keys, bool plain) {
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
    if (!plain) {
        t.compare_btrees(emptyKey);
        t.compare_btrees(firstKey);
    } else {
        t.scanThrough(emptyKey);
        t.scanThrough(firstKey);
    }

    // Double Insert
    for (uint64_t i = 0; i < (count*0.25); ++i) {
        t.insert(keys[i], keys[i]);
    }

    // Double Insert with different Values
    for (uint64_t i = (count*0.25), j = (count * 0.45); i < (count*0.5) && j >= 0; ++i, --j) {
        t.insert(keys[i], keys[j]);
        if (i == 38427 || i == 38426) {
            t.scanThrough(emptyKey);
        }
    }

    // Check Tree Structure
    if (!plain) {
        t.compare_btrees(emptyKey);
        t.compare_btrees(firstKey);
    } else {
        t.scanThrough(emptyKey);
        t.scanThrough(firstKey);
    }

    // Remove half
    for (uint64_t i = 0; i < (count*0.25); ++i) {
        t.remove(keys[i]);
    }

    // Insert firstKey and emptyKey (making sure there are still in btree)
    t.insert(firstKey, firstKey);
    t.insert(emptyKey, emptyKey);

    // Check Tree Structure
    if (!plain) {
        t.compare_btrees(emptyKey);
        t.compare_btrees(firstKey);
    } else {
        t.scanThrough(emptyKey);
        t.scanThrough(firstKey);
    }

    // Reshuffle Keys
    std::random_shuffle(keys.begin(), keys.end());

    // Insert half
    for (uint64_t i = 0; i < (count*0.5); ++i) {
        t.insert(keys[i], keys[i]);
    }

    // Check Tree Structure
    if (!plain) {
        t.compare_btrees(emptyKey);
        t.compare_btrees(firstKey);
    } else {
        t.scanThrough(emptyKey);
        t.scanThrough(firstKey);
    }

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
    if (!plain) {
        t.compare_btrees(emptyKey);
        t.compare_btrees(firstKey);
    } else {
        t.scanThrough(emptyKey);
        t.scanThrough(firstKey);
    }

    // Reshuffle Keys
    std::random_shuffle(keys.begin(), keys.end());
    
    // Insert all
    for (uint64_t i = 0; i < count; ++i) {
        t.insert(keys[i], keys[i]);
    }

    // Check Tree Structure
    if (!plain) {
        t.compare_btrees(emptyKey);
        t.compare_btrees(firstKey);
    } else {
        t.scanThrough(emptyKey);
        t.scanThrough(firstKey);
    }

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
    if (!plain) {
        t.compare_btrees(emptyKey);
        t.compare_btrees(firstKey);
    } else {
        t.scanThrough(emptyKey);
        t.scanThrough(firstKey);
    }

    // Remove all
    for (uint64_t i = 0; i < count; ++i) {
        t.remove(keys[i]);
    }

    // Check Tree Structure
    if (!plain) {
        t.compare_btrees(emptyKey);
        t.compare_btrees(firstKey);
    } else {
        t.scanThrough(emptyKey);
        t.scanThrough(firstKey);
    }

    std::cout << "runTestMixed PASSED" << std::endl;
}

void runPerformanceTestStandard(vector<vector<uint8_t>> &keys,PerfEvent& perf) {
    std::random_shuffle(keys.begin(), keys.end());
    
    TesterPerformance t{};
    TesterPerformanceTempl t_templ{};
    TesterPerformanceMap t_map;

    uint64_t count = keys.size();

    {
        PerfEventBlock peb(perf,count,{"insert"});
        for (uint64_t i = 0; i < count; ++i) {
            t.insert(keys[i], keys[i]);
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
        PerfEventBlock peb(perf,count,{"lookup"});
        for (uint64_t i = 0; i < count; ++i) {
            t.lookup(keys[i]);
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
        PerfEventBlock peb(perf,count,{"remove"});
        for (uint64_t i = 0; i < count; ++i) {
            t.remove(keys[i]);
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
    
    TesterPerformance t{};
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
        PerfEventBlock peb(perf,count,{"PerformanceTestMixed Eytzinger"});
        for (uint64_t i = 0; i < count; ++i) {
            t.insert(keys[i], keys[i]);
        }
        for (uint64_t i = 0; i < count; ++i) {
            t.lookup(keys[index2[i]]);
            t.remove(keys[index3[i]]);
            t.insert(keys[index3[i]], keys[index4[i]]);
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
    
    TesterPerformance t{};
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

    // Pre-Insert all keys in Eytzinger btree and the template for the Lookup performance test
    for (uint64_t i = 0; i < count; ++i) {
            t.insert(keys[i], keys[i]);
            t_templ.insert(keys[i], keys[i]);
        }
    
    {
        PerfEventBlock peb(perf,count,{"PerformanceTestLookup Eytzinger"});
        for (uint64_t i = 0; i < count; ++i) {
            t.lookup(keys[i]);
        }
        for (uint64_t i = 0; i < count; ++i) {
            t.lookup(keys[iteration2[i]]);
        }
        for (uint64_t i = 0; i < count; ++i) {
            t.lookup(keys[iteration3[i]]);
        }
    }
    {
        PerfEventBlock peb(perf,count,{"PerformanceLookup Template"});
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
        runTestMixed(data,true);
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
        runTestMixed(data,true);
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
        runTestMixed(data,true);
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
        runTestMixed(data,true);
    }
    return 0;
}