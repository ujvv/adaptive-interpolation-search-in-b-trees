#include "tester.hpp"
#include "PerfEvent.hpp"
#include <algorithm>
#include <csignal>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>
#include <numeric>

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
            //t.scanThrough(emptyKey);
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
    std::cout << "runTestStandard PASSED" << std::endl;
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
        /*if (i >= 19300) {
            t.scanThrough(emptyKey);
        }*/
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
    for (uint64_t i = 0, j = 0; i < count; ++i, j++) {
        t.insert(keys[i], keys[i]);
        /*if (i > 24000 && j >= 1000) {
            t.scanThrough(emptyKey);
            j = 0;
        }*/
        if (count == 100) {
            t.scanThrough(emptyKey);
        }
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
    std::cout << "PerformanceTestStandard START" << std::endl;
    
    std::random_shuffle(keys.begin(), keys.end());
    
    //TesterPerformanceMap t_map;
    //TesterPerformanceTempl t_templ{};
    //TesterPerformanceTemplBigNode t_templBigNode{};
    TesterPerformanceBinarySearchNoPrefix t_binarySearchNoPrefix{};
    TesterPerformanceBinarySearch t_binarySearch{};
    //TesterPerformanceBinarySearchBigNode t_binarySearchBigNode{};
    //TesterPerformanceBinarySearchHints t_binarySearchHints{};
    //TesterPerformanceLinearSearch t_linearSearch{};
    //TesterPerformanceInterpolationSearch t_interpolationSearch{};
    //TesterPerformanceInterpolationSearchKeyHeads t_interpolationSearchKeyHeads{};
    //TesterPerformanceInterpolationSearchKeyHeadsBigNode t_interpolationSearchKeyHeadsBigNode{};

    uint64_t count = keys.size();

    /*{
        PerfEventBlock peb(perf,count,{"insert stdMap"});
        for (uint64_t i = 0; i < count; ++i) {
            t_map.insert(keys[i], keys[i]);
        }
    }*/
    /*{
        PerfEventBlock peb(perf,count,{"insert Template"});
        for (uint64_t i = 0; i < count; ++i) {
            t_templ.insert(keys[i], keys[i]);
        }
    }*/
    /*{
        PerfEventBlock peb(perf,count,{"insert TemplateBigNode"});
        for (uint64_t i = 0; i < count; ++i) {
            t_templBigNode.insert(keys[i], keys[i]);
        }
    }*/
    {
        PerfEventBlock peb(perf,count,{"insert BinarySearchNoPrefix"});
        for (uint64_t i = 0; i < count; ++i) {
            t_binarySearchNoPrefix.insert(keys[i], keys[i]);
        }
    }
    {
        PerfEventBlock peb(perf,count,{"insert BinarySearch"});
        for (uint64_t i = 0; i < count; ++i) {
            t_binarySearch.insert(keys[i], keys[i]);
        }
    }
    /*{
        PerfEventBlock peb(perf,count,{"insert BinarySearchBigNode"});
        for (uint64_t i = 0; i < count; ++i) {
            t_binarySearchBigNode.insert(keys[i], keys[i]);
        }
    }*/
    /*{
        PerfEventBlock peb(perf,count,{"insert BinarySearch with Hints"});
        for (uint64_t i = 0; i < count; ++i) {
            t_binarySearchHints.insert(keys[i], keys[i]);
        }
    }*/
    /*{
        PerfEventBlock peb(perf,count,{"insert LinearSearch"});
        for (uint64_t i = 0; i < count; ++i) {
            t_linearSearch.insert(keys[i], keys[i]);
        }
    }*/
    /*{
        PerfEventBlock peb(perf,count,{"insert InterpolationSearch"});
        for (uint64_t i = 0; i < count; ++i) {
            t_interpolationSearch.insert(keys[i], keys[i]);
        }
    }*/
    /*{
        PerfEventBlock peb(perf,count,{"insert InterpolationSearchKeyHeads"});
        for (uint64_t i = 0; i < count; ++i) {
            t_interpolationSearchKeyHeads.insert(keys[i], keys[i]);
        }
    }*/
    /*{
        PerfEventBlock peb(perf,count,{"insert InterpolationSearchKeyHeadsBigNode"});
        for (uint64_t i = 0; i < count; ++i) {
            t_interpolationSearchKeyHeadsBigNode.insert(keys[i], keys[i]);
        }
    }*/


    /*{
        PerfEventBlock peb(perf,count,{"lookup stdMap"});
        for (uint64_t i = 0; i < count; ++i) {
            t_map.lookup(keys[i]);
        }
    }*/
    /*{
        PerfEventBlock peb(perf,count,{"lookup Template"});
        for (uint64_t i = 0; i < count; ++i) {
            t_templ.lookup(keys[i]);
        }
    }*/
    /*{
        PerfEventBlock peb(perf,count,{"lookup TemplateBigNode"});
        for (uint64_t i = 0; i < count; ++i) {
            t_templBigNode.lookup(keys[i]);
        }
    }*/
    {
        PerfEventBlock peb(perf,count,{"lookup BinarySearchNoPrefix"});
        for (uint64_t i = 0; i < count; ++i) {
            t_binarySearchNoPrefix.lookup(keys[i]);
        }
    }
    {
        PerfEventBlock peb(perf,count,{"lookup BinarySearch"});
        for (uint64_t i = 0; i < count; ++i) {
            t_binarySearch.lookup(keys[i]);
        }
    }
    /*{
        PerfEventBlock peb(perf,count,{"lookup BinarySearchBigNode"});
        for (uint64_t i = 0; i < count; ++i) {
            t_binarySearchBigNode.lookup(keys[i]);
        }
    }*/
    /*{
        PerfEventBlock peb(perf,count,{"lookup BinarySearch with Hints"});
        for (uint64_t i = 0; i < count; ++i) {
            t_binarySearchHints.lookup(keys[i]);
        }
    }*/
    /*{
        PerfEventBlock peb(perf,count,{"lookup LinearSearch"});
        for (uint64_t i = 0; i < count; ++i) {
            t_linearSearch.lookup(keys[i]);
        }
    }*/
    /*{
        PerfEventBlock peb(perf,count,{"lookup InterpolationSearch"});
        for (uint64_t i = 0; i < count; ++i) {
            t_interpolationSearch.lookup(keys[i]);
        }
    }*/
    /*{
        PerfEventBlock peb(perf,count,{"lookup InterpolationSearchKeyHeads"});
        for (uint64_t i = 0; i < count; ++i) {
            t_interpolationSearchKeyHeads.lookup(keys[i]);
        }
    }*/
    /*{
        PerfEventBlock peb(perf,count,{"lookup InterpolationSearchKeyHeadsBigNode"});
        for (uint64_t i = 0; i < count; ++i) {
            t_interpolationSearchKeyHeadsBigNode.lookup(keys[i]);
        }
    }*/

    /*{
        PerfEventBlock peb(perf,count,{"remove stdMap"});
        for (uint64_t i = 0; i < count; ++i) {
            t_map.remove(keys[i]);
        }
    }*/
    /*{
        PerfEventBlock peb(perf,count,{"remove Template"});
        for (uint64_t i = 0; i < count; ++i) {
            t_templ.remove(keys[i]);
        }
    }*/
    /*{
        PerfEventBlock peb(perf,count,{"remove TemplateBigNode"});
        for (uint64_t i = 0; i < count; ++i) {
            t_templBigNode.remove(keys[i]);
        }
    }*/
    {
        PerfEventBlock peb(perf,count,{"remove BinarySearchNoPrefix"});
        for (uint64_t i = 0; i < count; ++i) {
            t_binarySearchNoPrefix.remove(keys[i]);
        }
    }
    {
        PerfEventBlock peb(perf,count,{"remove BinarySearch"});
        for (uint64_t i = 0; i < count; ++i) {
            t_binarySearch.remove(keys[i]);
        }
    }
    /*{
        PerfEventBlock peb(perf,count,{"remove BinarySearchBigNode"});
        for (uint64_t i = 0; i < count; ++i) {
            t_binarySearchBigNode.remove(keys[i]);
        }
    }*/
    /*{
        PerfEventBlock peb(perf,count,{"remove BinarySearch with Hints"});
        for (uint64_t i = 0; i < count; ++i) {
            t_binarySearchHints.remove(keys[i]);
        }
    }*/
    /*{
        PerfEventBlock peb(perf,count,{"remove LinearSearch"});
        for (uint64_t i = 0; i < count; ++i) {
            t_binarySearch.remove(keys[i]);
        }
    }*/
    /*{
        PerfEventBlock peb(perf,count,{"remove InterpolationSearch"});
        for (uint64_t i = 0; i < count; ++i) {
            t_interpolationSearch.remove(keys[i]);
        }
    }*/
    /*{
        PerfEventBlock peb(perf,count,{"remove InterpolationSearchKeyHeads"});
        for (uint64_t i = 0; i < count; ++i) {
            t_interpolationSearchKeyHeads.remove(keys[i]);
        }
    }*/
    /*{
        PerfEventBlock peb(perf,count,{"remove InterpolationSearchKeyHeadsBigNode"});
        for (uint64_t i = 0; i < count; ++i) {
            t_interpolationSearchKeyHeadsBigNode.remove(keys[i]);
        }
    }*/

    std::cout << "PerformanceTestStandard END" << std::endl;
}

void runPerformanceTestMixed(vector<vector<uint8_t>> &keys,PerfEvent& perf) {
    std::cout << "PerformanceTestMixed START" << std::endl;
    
    std::random_shuffle(keys.begin(), keys.end());
    
    TesterPerformanceTempl t_templ{};
    //TesterPerformanceBinarySearch t_binarySearch{};
    TesterPerformanceBinarySearchHints t_binarySearchHints{};
    //TesterPerformanceLinearSearch t_linearSearch{};
    //TesterPerformanceInterpolationSearch t_interpolationSearch{};
    TesterPerformanceInterpolationSearchKeyHeads t_interpolationSearchKeyHeads{};

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
    /*{
        PerfEventBlock peb(perf,count,{"PerformanceTestMixed BinarySearch"});
        for (uint64_t i = 0; i < count; ++i) {
            t_binarySearch.insert(keys[i], keys[i]);
        }
        for (uint64_t i = 0; i < count; ++i) {
            t_binarySearch.lookup(keys[index2[i]]);
            t_binarySearch.remove(keys[index3[i]]);
            t_binarySearch.insert(keys[index3[i]], keys[index4[i]]);
        }
    }*/  
    {
        PerfEventBlock peb(perf,count,{"PerformanceTestMixed BinarySearch with Hints"});
        for (uint64_t i = 0; i < count; ++i) {
            t_binarySearchHints.insert(keys[i], keys[i]);
        }
        for (uint64_t i = 0; i < count; ++i) {
            t_binarySearchHints.lookup(keys[index2[i]]);
            t_binarySearchHints.remove(keys[index3[i]]);
            t_binarySearchHints.insert(keys[index3[i]], keys[index4[i]]);
        }
    }
    /*{
        PerfEventBlock peb(perf,count,{"PerformanceTestMixed LinearSearch"});
        for (uint64_t i = 0; i < count; ++i) {
            t_linearSearch.insert(keys[i], keys[i]);
        }
        for (uint64_t i = 0; i < count; ++i) {
            t_linearSearch.lookup(keys[index2[i]]);
            t_linearSearch.remove(keys[index3[i]]);
            t_linearSearch.insert(keys[index3[i]], keys[index4[i]]);
        }
    }
    {
        PerfEventBlock peb(perf,count,{"PerformanceTestMixed InterpolationSearch"});
        for (uint64_t i = 0; i < count; ++i) {
            t_interpolationSearch.insert(keys[i], keys[i]);
        }
        for (uint64_t i = 0; i < count; ++i) {
            t_interpolationSearch.lookup(keys[index2[i]]);
            t_interpolationSearch.remove(keys[index3[i]]);
            t_interpolationSearch.insert(keys[index3[i]], keys[index4[i]]);
        }
    }*/
    {
        PerfEventBlock peb(perf,count,{"PerformanceTestMixed InterpolationSearchKeyHeads"});
        for (uint64_t i = 0; i < count; ++i) {
            t_interpolationSearchKeyHeads.insert(keys[i], keys[i]);
        }
        for (uint64_t i = 0; i < count; ++i) {
            t_interpolationSearchKeyHeads.lookup(keys[index2[i]]);
            t_interpolationSearchKeyHeads.remove(keys[index3[i]]);
            t_interpolationSearchKeyHeads.insert(keys[index3[i]], keys[index4[i]]);
        }
    }
    std::cout << "PerformanceTestMixed END" << std::endl;
}

void runPerformanceTestLookup(vector<vector<uint8_t>> &keys,PerfEvent& perf) {
    std::cout << "PerformanceTestLookup START" << std::endl;
    
    std::random_shuffle(keys.begin(), keys.end());
    
    //TesterPerformanceTempl t_templ{};
    TesterPerformanceBinarySearch t_binarySearch{};
    //TesterPerformanceBinarySearchHints t_binarySearchHints{};
    //TesterPerformanceLinearSearch t_linearSearch{};
    TesterPerformanceInterpolationSearch t_interpolationSearch{};
    TesterPerformanceInterpolationSearchKeyHeads t_interpolationSearchKeyHeads{};

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
    /*for (uint64_t i = 0; i < count; ++i) {
            //t_binarySearch.insert(keys[i], keys[i]);
            t_templ.insert(keys[i], keys[i]);
        }
    {
        PerfEventBlock peb(perf,count,{"PerformanceTestLookup Template"});
        for (uint64_t i = 0; i < count; ++i) {
            t_templ.lookup(keys[i]);
        }
        for (uint64_t i = 0; i < count; ++i) {
            t_templ.lookup(keys[iteration2[i]]);
        }
        for (uint64_t i = 0; i < count; ++i) {
            t_templ.lookup(keys[iteration3[i]]);
        }
    }*/
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
    /*{
        PerfEventBlock peb(perf,count,{"PerformanceTestLookup BinarySearch with Hints"});
        for (uint64_t i = 0; i < count; ++i) {
            t_binarySearchHints.lookup(keys[i]);
        }
        for (uint64_t i = 0; i < count; ++i) {
            t_binarySearchHints.lookup(keys[iteration2[i]]);
        }
        for (uint64_t i = 0; i < count; ++i) {
            t_binarySearchHints.lookup(keys[iteration3[i]]);
        }
    }*/
    /*{
        PerfEventBlock peb(perf,count,{"PerformanceTestLookup LinearSearch"});
        for (uint64_t i = 0; i < count; ++i) {
            t_linearSearch.lookup(keys[i]);
        }
        for (uint64_t i = 0; i < count; ++i) {
            t_linearSearch.lookup(keys[iteration2[i]]);
        }
        for (uint64_t i = 0; i < count; ++i) {
            t_linearSearch.lookup(keys[iteration3[i]]);
        }
    }*/
    {
        PerfEventBlock peb(perf,count,{"PerformanceTestLookup InterpolationSearch"});
        for (uint64_t i = 0; i < count; ++i) {
            t_interpolationSearch.lookup(keys[i]);
        }
        for (uint64_t i = 0; i < count; ++i) {
            t_interpolationSearch.lookup(keys[iteration2[i]]);
        }
        for (uint64_t i = 0; i < count; ++i) {
            t_interpolationSearch.lookup(keys[iteration3[i]]);
        }
    }
    {
        PerfEventBlock peb(perf,count,{"PerformanceTestLookup InterpolationSearchKeyHeads"});
        for (uint64_t i = 0; i < count; ++i) {
            t_interpolationSearchKeyHeads.lookup(keys[i]);
        }
        for (uint64_t i = 0; i < count; ++i) {
            t_interpolationSearchKeyHeads.lookup(keys[iteration2[i]]);
        }
        for (uint64_t i = 0; i < count; ++i) {
            t_interpolationSearchKeyHeads.lookup(keys[iteration3[i]]);
        }
    }
    std::cout << "PerformanceTestLookup END" << std::endl;
}

void verifyBTreeNodeSizes() {
    Tester t{};

    try {
        if (t.btree->root->CONTENT_SIZE != t.btreeTemplate->root->CONTENT_SIZE) {
            std::cout << "Varying Node Sizes in Tester. Template Content Size: " << t.btreeTemplate->root->CONTENT_SIZE << ", Tested BTree Content Size: " << t.btree->root->CONTENT_SIZE << std::endl;
        }
    } catch(...) {
        // If btree_plain is being selected as the tested Btree calling root->CONTENT_SIZE will not work
        std::cout << "btree_plain being tested" << std::endl;
    }

    TesterPerformanceTempl t_templ{};
    TesterPerformanceBinarySearchNoPrefix t_binarySearchNoPrefix{};
    TesterPerformanceBinarySearch t_binarySearch{};
    TesterPerformanceBinarySearchHints t_binarySearchHints{};
    TesterPerformanceLinearSearch t_linearSearch{};
    TesterPerformanceInterpolationSearch t_interpolationSearch{};
    TesterPerformanceInterpolationSearchKeyHeads t_interpolationSearchKeyHeads{};

    uint32_t template_ContentSize = t_templ.btreeTemplate->root->CONTENT_SIZE;
    uint32_t binarySearchNoPrefix_ContentSize = t_binarySearchNoPrefix.btreeBinarySearchNoPrefix->root->CONTENT_SIZE;
    uint32_t binarySearch_ContentSize = t_binarySearch.btreeBinarySearch->root->CONTENT_SIZE;
    uint32_t binarySearchHints_ContentSize = t_binarySearchHints.btreeBinarySearchHints->root->CONTENT_SIZE;
    uint32_t linearSearch_ContentSize = t_linearSearch.btreeLinearSearch->root->CONTENT_SIZE;
    uint32_t interpolationSearch_ContentSize = t_interpolationSearch.btreeInterpolationSearch->root->CONTENT_SIZE;
    uint32_t interpolationSearchKeyHeads_ContentSize = t_interpolationSearchKeyHeads.btreeInterpolationSearchKeyHeads->root->CONTENT_SIZE;

    if (binarySearch_ContentSize != binarySearchNoPrefix_ContentSize || binarySearch_ContentSize != binarySearchHints_ContentSize || binarySearch_ContentSize != linearSearch_ContentSize || binarySearch_ContentSize != interpolationSearch_ContentSize || binarySearch_ContentSize != interpolationSearchKeyHeads_ContentSize) {
        std::cout << "BTree Node Size Inconsistency. Content Sizes:   Template=" << template_ContentSize << ", BinarySearch=" << binarySearch_ContentSize << ", BinarySearchNoPrefix=" << binarySearchNoPrefix_ContentSize << ", BinarySearchHints=" << binarySearchHints_ContentSize << ", LinearSearch=" << linearSearch_ContentSize << ", InterpolationSearch=" << interpolationSearch_ContentSize << ", InterpolationSearchKeyHeads=" << interpolationSearchKeyHeads_ContentSize << std::endl;
    } else if (binarySearch_ContentSize != template_ContentSize) {
        std::cout << "BTreeTemplate has a different Node Size. BTreeTemplate Content Size: " << template_ContentSize << ", other Btree Content Sizes: " << binarySearch_ContentSize << std::endl;
    } else {
        std::cout << "BTrees CONTENT_SIZE = " << template_ContentSize << std::endl;
    }
}

std::vector<uint8_t> stringToVector(const std::string &str) {
    return std::vector<uint8_t>(str.begin(), str.end());
}

std::vector<uint64_t> fal(double shape, uint64_t size) {
        
}

int main() {
    srand(42);
    PerfEvent perf;

    verifyBTreeNodeSizes();

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
        runPerformanceTestStandard(data, perf);
        //runPerformanceTestMixed(data, perf);
        //runPerformanceTestLookup(data, perf);
        //runTest(data);
        //runTestReverse(data);
        //runTestZickZack(data);
        //runTestMixed(data);
    }

    if (getenv("INTUNIFORM")) {
        vector<vector<uint8_t>> data;
        uint64_t n = atof(getenv("INTUNIFORM"));
        uint8_t byte1 = 0;
        uint8_t byte2 = 0;
        uint8_t byte3 = 0;
        uint8_t byte4 = 0;
        for (uint64_t i = 0; i < n; i++) {
            uint8_t bytes[4];
            bytes[0] = byte4;
            bytes[1] = byte3;
            bytes[2] = byte2;
            bytes[3] = byte1;
            data.emplace_back(bytes, bytes + 4);
            if (byte1 == 255) {
                if (byte2 == 255) {
                    if (byte3 == 255) {
                        byte4++;
                    }
                    byte3++;
                }
                byte2++;
            }
            byte1++;
        }
        //runPerformanceTestStandard(data, perf);
        //runPerformanceTestMixed(data, perf);
        //runPerformanceTestLookup(data, perf);
        runTest(data);
        //runTestReverse(data);
        //runTestZickZack(data);
        //runTestMixed(data);
    }

    if (getenv("INTFAL")) {
        vector<vector<uint8_t>> data;
        uint64_t n = atof(getenv("INTFAL"));
        std::vector<uint64_t> v(n);
        double shape = 0.5;
        auto n = v.size();
        for (auto i = 0; i < v.size() - 1; i++) {
            // scale up to ensure elements are distinct for as long as possible
             v[i] = pow((double)(n - i), - shape) * std::numeric_limits<uint64_t>::max();
        }
        v.back() = std::numeric_limits<uint64_t>::max();
        //runPerformanceTestStandard(data, perf);
        //runPerformanceTestMixed(data, perf);
        //runPerformanceTestLookup(data, perf); 
        //runTest(data);
        //runTestReverse(data);
        //runTestZickZack(data);
        //runTestMixed(data);
    }

    if (getenv("INTCFAL")) {
        vector<vector<uint8_t>> data;
        uint64_t n = atof(getenv("INTCFAL"));
        std::vector<uint64_t> v(n);
        double shape = 0.5;
        auto n = v.size();
        auto v = fal(shape, n);
        auto max_sum = std::accumulate(v.begin(), v.end(), 0.0L);
        auto scale = std::numeric_limits<uint64_t>::max() / max_sum;
        std::transform(v.begin(), v.end(), v.begin(),
                   [scale](auto x) { return x * scale; });
        std::partial_sum(v.begin(), v.end(), v.begin());
        //runPerformanceTestStandard(data, perf);
        //runPerformanceTestMixed(data, perf);
        //runPerformanceTestLookup(data, perf);
        //runTest(data);
        //runTestReverse(data);
        //runTestZickZack(data);
        //runTestMixed(data);
    }

    if (getenv("INTRANDOM")) {
        vector<vector<uint8_t>> data;
        uint64_t n = atof(getenv("INTRANDOM"));
        for (uint64_t i = 0; i < n; i++) {
            uint8_t bytes[4];
            bytes[0] = rand() % 255;
            bytes[1] = rand() % 255;
            bytes[2] = rand() % 255;
            bytes[3] = rand() % 255;
            data.emplace_back(bytes, bytes + 4);
        }
        //runPerformanceTestStandard(data, perf);
        //runPerformanceTestMixed(data, perf);
        //runPerformanceTestLookup(data, perf);
        //runTest(data);
        //runTestReverse(data);
        //runTestZickZack(data);
        //runTestMixed(data);
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
        //runPerformanceTestStandard(data, perf);
        //runPerformanceTestMixed(data, perf);
        //runPerformanceTestLookup(data, perf);
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