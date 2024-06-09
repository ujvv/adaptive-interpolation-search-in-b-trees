#include "test_main.hpp"
#include "tester.hpp"
#include "PerfEvent.hpp"

#include <algorithm>
#include <csignal>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>
#include <numeric>
#include <random>

using namespace std;


void runSingleNodeDistributionByteKeys(vector<vector<uint8_t>> &keys, uint64_t count, bool sort) {
    vector<uint32_t> keysInts;
    if (keys.size() < count) {
        count = keys.size();
    }
    for (uint64_t i = 0; i < count; i++) {
        uint32_t keyHead = 0;
        vector<uint8_t> key = keys.at(i);
        if (key.size() > 0) {
            std::memcpy(&keyHead, key.data(), std::min(key.size(), sizeof(uint32_t)));
            if constexpr (std::endian::native == std::endian::little) {
                keyHead = ((keyHead & 0xFF000000) >> 24) | ((keyHead & 0x00FF0000) >> 8) | ((keyHead & 0x0000FF00) << 8) | ((keyHead & 0x000000FF) << 24);
            }
        }
        keysInts.push_back(keyHead);
    }
    runSingleNodeDistribution(keysInts, count, sort);
}

void runSingleNodeDistribution(vector<uint32_t> &keys, uint64_t count, bool sort) {
    BtreeAnalysis t{};
    if (sort) {
        std::sort(keys.begin(), keys.end());
    }
    if (keys.size() < count) {
        count = keys.size();
    }
    std::vector<uint32_t> keyDifferences = t.calculateKeyDifferences(keys, count);
    double mean = t.mean(keyDifferences);
    double standardDeviation = t.standardDeviation(keyDifferences, mean);
    double coefficientOfVariation = standardDeviation / mean;
    std::cout << "Coefficient of Variation: " << coefficientOfVariation << std::endl;
}

void runAnalysis(vector<vector<uint8_t>> &keys) {
    uint64_t count = keys.size();

    BtreeAnalysis t{};

    for (uint64_t i = 0; i < count; i++) {
        t.insert(keys[i], keys[i]);
    }
    std::vector<double> coefficientsOfVariationLeafs = t.analyzeLeafs();
    std::sort(coefficientsOfVariationLeafs.begin(), coefficientsOfVariationLeafs.end());
    uint64_t coefficientAmountLeafs = coefficientsOfVariationLeafs.size();
    std::cout << "Leaf Analysis with " << coefficientAmountLeafs << " Leafs in total" << std::endl;
    std::vector<uint32_t> numKeysInLeafs = t.numKeysLeafs();
    double meanNumKeysInLeafs = t.mean(numKeysInLeafs);
    std::sort(numKeysInLeafs.begin(), numKeysInLeafs.end());
    double medianNumKeysInLeafs = t.median(numKeysInLeafs);
    std::cout << "Average amount of Keys in a Leaf: " << meanNumKeysInLeafs << " Median numKeys in Leafs: " << medianNumKeysInLeafs << std::endl;
    std::cout << "The 5 lowest numKeys in a Leaf: ";
    for (int i = 0; i < numKeysInLeafs.size() && i < 5; i++) {
        std::cout << numKeysInLeafs[i] <<" ";
    }
    std::cout << ".  And the 5 highest numKeys in a Leaf: ";
    for (int i = 0; i < numKeysInLeafs.size() && i < 5; i++) {
        std::cout << numKeysInLeafs[numKeysInLeafs.size() -1 -i] <<" ";
    }

    double meanCoefficientLeafs = t.mean(coefficientsOfVariationLeafs);
    double medianCoefficientLeafs = t.median(coefficientsOfVariationLeafs);
    double standardDeviationOfCoefficientsLeafs = t.standardDeviationOfCoeffecients(coefficientsOfVariationLeafs, meanCoefficientLeafs);
    std::cout << "Mean of Coefficients: " << meanCoefficientLeafs << " Median of Coefficients: " << medianCoefficientLeafs << " StandardDeviation of Coefficients: " << standardDeviationOfCoefficientsLeafs<< std::endl;
    double lowestCoefficientLeafs = coefficientsOfVariationLeafs[0];
    double highestCoefficientLeafs = coefficientsOfVariationLeafs[coefficientsOfVariationLeafs.size() -1];
    std::cout << "The 5 lowest Coefficients: ";
    for (int i = 0; i < coefficientAmountLeafs && i < 5; i++) {
        std::cout << coefficientsOfVariationLeafs[i] <<" ";
    }
    std::cout << std::endl;
    std::cout << "The 5 highest Coefficients: ";
    for (int i = 0; i < coefficientAmountLeafs && i < 5; i++) {
        std::cout << coefficientsOfVariationLeafs[coefficientsOfVariationLeafs.size() -1 -i] <<" ";
    }
    std::cout << std::endl;
    

    
    std::vector<double> coefficientsOfVariationInnerNodes = t.analyzeInnerNodes();
    std::sort(coefficientsOfVariationInnerNodes.begin(), coefficientsOfVariationInnerNodes.end());
    uint64_t coefficientAmountInnerNodes = coefficientsOfVariationInnerNodes.size();
    std::cout << "InnerNode Analysis with " << coefficientAmountInnerNodes << " InnerNodes in total" << std::endl;
    std::vector<uint32_t> numKeysInInnerNodes = t.numKeysInnerNodes();
    double meanNumKeysInInnerNodes = t.mean(numKeysInInnerNodes);
    std::sort(numKeysInInnerNodes.begin(), numKeysInInnerNodes.end());
    double medianNumKeysInInnerNodes = t.median(numKeysInInnerNodes);
    std::cout << "Average amount of Keys in a InnerNode: " << meanNumKeysInInnerNodes << " Median numKeys in InnerNodes: " << medianNumKeysInInnerNodes << std::endl;
    std::cout << "The 5 lowest numKeys in a InnerNode: ";
    for (int i = 0; i < numKeysInLeafs.size() && i < 5; i++) {
        std::cout << numKeysInLeafs[i] <<" ";
    }
    std::cout << ".  And the 5 highest numKeys in a InnerNode: ";
    for (int i = 0; i < numKeysInLeafs.size() && i < 5; i++) {
        std::cout << numKeysInLeafs[numKeysInLeafs.size() -1 -i] <<" ";
    }

    double meanCoefficientInnerNodes = t.mean(coefficientsOfVariationInnerNodes);
    double medianCoefficientInnerNodes = t.median(coefficientsOfVariationInnerNodes);
    double standardDeviationOfCoefficientsInnerNodes = t.standardDeviationOfCoeffecients(coefficientsOfVariationInnerNodes, meanCoefficientInnerNodes);
    std::cout << "Mean of Coefficients: " << meanCoefficientInnerNodes << " Median of Coefficients: " << medianCoefficientInnerNodes << " StandardDeviation of Coefficients: " << standardDeviationOfCoefficientsInnerNodes<< std::endl;
    double lowestCoefficientInnerNodes = coefficientsOfVariationInnerNodes[0];
    double highestCoefficientInnerNodes = coefficientsOfVariationInnerNodes[coefficientsOfVariationInnerNodes.size() -1];
    std::cout << "The 5 lowest Coefficients: ";
    for (int i = 0; i < coefficientAmountInnerNodes && i < 5; i++) {
        std::cout << coefficientsOfVariationInnerNodes[i] <<" ";
    }
    std::cout << std::endl;
    std::cout << "The 5 highest Coefficients: ";
    for (int i = 0; i < coefficientAmountInnerNodes && i < 5; i++) {
        std::cout << coefficientsOfVariationInnerNodes[coefficientsOfVariationInnerNodes.size() -1 -i] <<" ";
    }
    std::cout << std::endl;
}

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
    for (uint64_t i = (count*0.25), j = (count * 0.45); i < (count*0.5) && j > 0; ++i, --j) {
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

void runPerformanceTestStandard(vector<vector<uint8_t>> &keys, PerfEvent& perf, Btrees type) {
    std::cout << "PerformanceTestStandard START" << std::endl;
    
    std::random_shuffle(keys.begin(), keys.end());
    
    uint64_t count = keys.size();

    if (type == MAP) {
        TesterPerformanceMap t_map;
        {
            PerfEventBlock peb(perf,count,{"insert stdMap"});
            for (uint64_t i = 0; i < count; ++i) {
                t_map.insert(keys[i], keys[i]);
            }
        }
        {
            PerfEventBlock peb(perf,count,{"lookup stdMap"});
            for (uint64_t i = 0; i < count; ++i) {
                t_map.lookup(keys[i]);
            }
        }
        {
            PerfEventBlock peb(perf,count,{"remove stdMap"});
            for (uint64_t i = 0; i < count; ++i) {
                t_map.remove(keys[i]);
            }
        }
    } else if (type == TEMPLATE) {
        TesterPerformanceTempl t_templ{};
        {
            PerfEventBlock peb(perf,count,{"insert Template"});
            for (uint64_t i = 0; i < count; ++i) {
                t_templ.insert(keys[i], keys[i]);
            }
        }
        {
            PerfEventBlock peb(perf,count,{"lookup Template"});
            for (uint64_t i = 0; i < count; ++i) {
                t_templ.lookup(keys[i]);
            }
        }
        {
            PerfEventBlock peb(perf,count,{"remove Template"});
            for (uint64_t i = 0; i < count; ++i) {
                t_templ.remove(keys[i]);
            }
        }
    } else if (type == TEMPLATEBIGNODE) {
        TesterPerformanceTemplBigNode t_templBigNode{};
        {
            PerfEventBlock peb(perf,count,{"insert TemplateBigNode"});
            for (uint64_t i = 0; i < count; ++i) {
                t_templBigNode.insert(keys[i], keys[i]);
            }
        }
        {
            PerfEventBlock peb(perf,count,{"lookup TemplateBigNode"});
            for (uint64_t i = 0; i < count; ++i) {
                t_templBigNode.lookup(keys[i]);
            }
        }
        {
            PerfEventBlock peb(perf,count,{"remove TemplateBigNode"});
            for (uint64_t i = 0; i < count; ++i) {
                t_templBigNode.remove(keys[i]);
            }
        }
    } else if (type == BINARYSEARCH) {
        TesterPerformanceBinarySearch t_binarySearch{};
        {
            PerfEventBlock peb(perf,count,{"insert BinarySearch"});
            for (uint64_t i = 0; i < count; ++i) {
                t_binarySearch.insert(keys[i], keys[i]);
            }
        }
        {
            PerfEventBlock peb(perf,count,{"lookup BinarySearch"});
            for (uint64_t i = 0; i < count; ++i) {
                t_binarySearch.lookup(keys[i]);
            }
        }
        {
            PerfEventBlock peb(perf,count,{"remove BinarySearch"});
            for (uint64_t i = 0; i < count; ++i) {
                t_binarySearch.remove(keys[i]);
            }
        }
    } else if (type == BINARYSEARCHBIGNODE) {
        TesterPerformanceBinarySearchBigNode t_binarySearchBigNode{};        
        {
            PerfEventBlock peb(perf,count,{"insert BinarySearchBigNode"});
            for (uint64_t i = 0; i < count; ++i) {
                t_binarySearchBigNode.insert(keys[i], keys[i]);
            }
        }
        {
            PerfEventBlock peb(perf,count,{"lookup BinarySearchBigNode"});
            for (uint64_t i = 0; i < count; ++i) {
                t_binarySearchBigNode.lookup(keys[i]);
            }
        }
        {
            PerfEventBlock peb(perf,count,{"remove BinarySearchBigNode"});
            for (uint64_t i = 0; i < count; ++i) {
                t_binarySearchBigNode.remove(keys[i]);
            }
        }
    } else if (type == BINARYSEARCHWITHHINTS) {
        TesterPerformanceBinarySearchHints t_binarySearchHints{};      
        {
            PerfEventBlock peb(perf,count,{"insert BinarySearch with Hints"});
            for (uint64_t i = 0; i < count; ++i) {
                t_binarySearchHints.insert(keys[i], keys[i]);
            }
        }
        {
            PerfEventBlock peb(perf,count,{"lookup BinarySearch with Hints"});
            for (uint64_t i = 0; i < count; ++i) {
                t_binarySearchHints.lookup(keys[i]);
            }
        }
        {
            PerfEventBlock peb(perf,count,{"remove BinarySearch with Hints"});
            for (uint64_t i = 0; i < count; ++i) {
                t_binarySearchHints.remove(keys[i]);
            }
        }
    } else if (type == BINARYSEARCHNOPREFIX) {
        TesterPerformanceBinarySearchNoPrefix t_binarySearchNoPrefix{};       
        {
            PerfEventBlock peb(perf,count,{"insert BinarySearchNoPrefix"});
            for (uint64_t i = 0; i < count; ++i) {
                t_binarySearchNoPrefix.insert(keys[i], keys[i]);
            }
        }
        {
            PerfEventBlock peb(perf,count,{"lookup BinarySearchNoPrefix"});
            for (uint64_t i = 0; i < count; ++i) {
                t_binarySearchNoPrefix.lookup(keys[i]);
            }
        }
        {
            PerfEventBlock peb(perf,count,{"remove BinarySearchNoPrefix"});
            for (uint64_t i = 0; i < count; ++i) {
                t_binarySearchNoPrefix.remove(keys[i]);
            }
        }
    } else if (type == LINEARSEARCH) {
        TesterPerformanceLinearSearch t_linearSearch{};     
        {
            PerfEventBlock peb(perf,count,{"insert LinearSearch"});
            for (uint64_t i = 0; i < count; ++i) {
                t_linearSearch.insert(keys[i], keys[i]);
            }
        }
        {
            PerfEventBlock peb(perf,count,{"lookup LinearSearch"});
            for (uint64_t i = 0; i < count; ++i) {
                t_linearSearch.lookup(keys[i]);
            }
        }
        {
            PerfEventBlock peb(perf,count,{"remove LinearSearch"});
            for (uint64_t i = 0; i < count; ++i) {
                t_linearSearch.remove(keys[i]);
            }
        }
    } else if (type == INTERPOLATIONSEARCH) {
        TesterPerformanceInterpolationSearch t_interpolationSearch{};    
        {
            PerfEventBlock peb(perf,count,{"insert InterpolationSearch"});
            for (uint64_t i = 0; i < count; ++i) {
                t_interpolationSearch.insert(keys[i], keys[i]);
            }
        }
        {
            PerfEventBlock peb(perf,count,{"lookup InterpolationSearch"});
            for (uint64_t i = 0; i < count; ++i) {
                t_interpolationSearch.lookup(keys[i]);
            }
        }
        {
            PerfEventBlock peb(perf,count,{"remove InterpolationSearch"});
            for (uint64_t i = 0; i < count; ++i) {
                t_interpolationSearch.remove(keys[i]);
            }
        }
    } else if (type == INTERPOLATIONSEARCHBIGNODE) {
        TesterPerformanceInterpolationSearchBigNode t_interpolationSearchBigNode{};    
        {
            PerfEventBlock peb(perf,count,{"insert InterpolationSearchBigNode"});
            for (uint64_t i = 0; i < count; ++i) {
                t_interpolationSearchBigNode.insert(keys[i], keys[i]);
            }
        }
        {
            PerfEventBlock peb(perf,count,{"lookup InterpolationSearchBigNode"});
            for (uint64_t i = 0; i < count; ++i) {
                t_interpolationSearchBigNode.lookup(keys[i]);
            }
        }
        {
            PerfEventBlock peb(perf,count,{"remove InterpolationSearchBigNode"});
            for (uint64_t i = 0; i < count; ++i) {
                t_interpolationSearchBigNode.remove(keys[i]);
            }
        }
    } else if (type == INTERPOLATIONSEARCHWITHKEYHEADS) {
        TesterPerformanceInterpolationSearchKeyHeads t_interpolationSearchKeyHeads{};  
        {
            PerfEventBlock peb(perf,count,{"insert InterpolationSearchKeyHeads"});
            for (uint64_t i = 0; i < count; ++i) {
                t_interpolationSearchKeyHeads.insert(keys[i], keys[i]);
            }
        }
        {
            PerfEventBlock peb(perf,count,{"lookup InterpolationSearchKeyHeads"});
            for (uint64_t i = 0; i < count; ++i) {
                t_interpolationSearchKeyHeads.lookup(keys[i]);
            }
        }
        {
            PerfEventBlock peb(perf,count,{"remove InterpolationSearchKeyHeads"});
            for (uint64_t i = 0; i < count; ++i) {
                t_interpolationSearchKeyHeads.remove(keys[i]);
            }
        }
    } else if (type == INTERPOLATIONSEARCHWITHKEYHEADSBIGNODE) {
        TesterPerformanceInterpolationSearchKeyHeadsBigNode t_interpolationSearchKeyHeadsBigNode{};
        {
            PerfEventBlock peb(perf,count,{"insert InterpolationSearchKeyHeadsBigNode"});
            for (uint64_t i = 0; i < count; ++i) {
                t_interpolationSearchKeyHeadsBigNode.insert(keys[i], keys[i]);
            }
        }
        {
            PerfEventBlock peb(perf,count,{"lookup InterpolationSearchKeyHeadsBigNode"});
            for (uint64_t i = 0; i < count; ++i) {
                t_interpolationSearchKeyHeadsBigNode.lookup(keys[i]);
            }
        }
        {
            PerfEventBlock peb(perf,count,{"remove InterpolationSearchKeyHeadsBigNode"});
            for (uint64_t i = 0; i < count; ++i) {
                t_interpolationSearchKeyHeadsBigNode.remove(keys[i]);
            }
        }
    } else {
        std::cout << "MISSING" << std::endl;
    }

    std::cout << "PerformanceTestStandard END" << std::endl;
}

void runPerformanceTestMixed(vector<vector<uint8_t>> &keys,PerfEvent& perf, Btrees type) {
    std::cout << "PerformanceTestMixed START" << std::endl;
    
    std::random_shuffle(keys.begin(), keys.end());

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

    if (type == MAP) {
        TesterPerformanceMap t_map;
        {
            PerfEventBlock peb(perf,count,{"PerformanceTestMixed stdMap"});
            for (uint64_t i = 0; i < count; ++i) {
                t_map.insert(keys[i], keys[i]);
            }
            for (uint64_t i = 0; i < count; ++i) {
                t_map.lookup(keys[index2[i]]);
                t_map.remove(keys[index3[i]]);
                t_map.insert(keys[index3[i]], keys[index4[i]]);
            }
        } 
    } else if (type == TEMPLATE) {
        TesterPerformanceTempl t_templ{};
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
    } else if (type == TEMPLATEBIGNODE) {
        TesterPerformanceTemplBigNode t_templBigNode{};
        {
            PerfEventBlock peb(perf,count,{"PerformanceTestMixed TemplateBigNode"});
            for (uint64_t i = 0; i < count; ++i) {
                t_templBigNode.insert(keys[i], keys[i]);
            }
            for (uint64_t i = 0; i < count; ++i) {
                t_templBigNode.lookup(keys[index2[i]]);
                t_templBigNode.remove(keys[index3[i]]);
                t_templBigNode.insert(keys[index3[i]], keys[index4[i]]);
            }
        } 
    } else if (type == BINARYSEARCH) {
        TesterPerformanceBinarySearch t_binarySearch{};
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
    } else if (type == BINARYSEARCHBIGNODE) {
        TesterPerformanceBinarySearchBigNode t_binarySearchBigNode{};        
        {
            PerfEventBlock peb(perf,count,{"PerformanceTestMixed BinarySearchBigNode"});
            for (uint64_t i = 0; i < count; ++i) {
                t_binarySearchBigNode.insert(keys[i], keys[i]);
            }
            for (uint64_t i = 0; i < count; ++i) {
                t_binarySearchBigNode.lookup(keys[index2[i]]);
                t_binarySearchBigNode.remove(keys[index3[i]]);
                t_binarySearchBigNode.insert(keys[index3[i]], keys[index4[i]]);
            }
        } 
    } else if (type == BINARYSEARCHWITHHINTS) {
        TesterPerformanceBinarySearchHints t_binarySearchHints{};      
        {
            PerfEventBlock peb(perf,count,{"PerformanceTestMixed BinarySearchHints"});
            for (uint64_t i = 0; i < count; ++i) {
                t_binarySearchHints.insert(keys[i], keys[i]);
            }
            for (uint64_t i = 0; i < count; ++i) {
                t_binarySearchHints.lookup(keys[index2[i]]);
                t_binarySearchHints.remove(keys[index3[i]]);
                t_binarySearchHints.insert(keys[index3[i]], keys[index4[i]]);
            }
        } 
    } else if (type == BINARYSEARCHNOPREFIX) {
        TesterPerformanceBinarySearchNoPrefix t_binarySearchNoPrefix{};       
        {
            PerfEventBlock peb(perf,count,{"PerformanceTestMixed BinarySearchNoPrefix"});
            for (uint64_t i = 0; i < count; ++i) {
                t_binarySearchNoPrefix.insert(keys[i], keys[i]);
            }
            for (uint64_t i = 0; i < count; ++i) {
                t_binarySearchNoPrefix.lookup(keys[index2[i]]);
                t_binarySearchNoPrefix.remove(keys[index3[i]]);
                t_binarySearchNoPrefix.insert(keys[index3[i]], keys[index4[i]]);
            }
        } 
    } else if (type == LINEARSEARCH) {
        TesterPerformanceLinearSearch t_linearSearch{};     
        {
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
    } else if (type == INTERPOLATIONSEARCH) {
        TesterPerformanceInterpolationSearch t_interpolationSearch{};    
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
        } 
    } else if (type == INTERPOLATIONSEARCHBIGNODE) {
        TesterPerformanceInterpolationSearchBigNode t_interpolationSearchBigNode{};    
        {
            PerfEventBlock peb(perf,count,{"PerformanceTestMixed InterpolationSearchBigNode"});
            for (uint64_t i = 0; i < count; ++i) {
                t_interpolationSearchBigNode.insert(keys[i], keys[i]);
            }
            for (uint64_t i = 0; i < count; ++i) {
                t_interpolationSearchBigNode.lookup(keys[index2[i]]);
                t_interpolationSearchBigNode.remove(keys[index3[i]]);
                t_interpolationSearchBigNode.insert(keys[index3[i]], keys[index4[i]]);
            }
        } 
    } else if (type == INTERPOLATIONSEARCHWITHKEYHEADS) {
        TesterPerformanceInterpolationSearchKeyHeads t_interpolationSearchKeyHeads{};  
        {
            PerfEventBlock peb(perf,count,{"PerformanceTestMixed InterpolationSearch with KeyHeads"});
            for (uint64_t i = 0; i < count; ++i) {
                t_interpolationSearchKeyHeads.insert(keys[i], keys[i]);
            }
            for (uint64_t i = 0; i < count; ++i) {
                t_interpolationSearchKeyHeads.lookup(keys[index2[i]]);
                t_interpolationSearchKeyHeads.remove(keys[index3[i]]);
                t_interpolationSearchKeyHeads.insert(keys[index3[i]], keys[index4[i]]);
            }
        } 
    } else if (type == INTERPOLATIONSEARCHWITHKEYHEADSBIGNODE) {
        TesterPerformanceInterpolationSearchKeyHeadsBigNode t_interpolationSearchKeyHeadsBigNode{};
        {
            PerfEventBlock peb(perf,count,{"PerformanceTestMixed InterpolationSearch with KeyHeadsBigNode"});
            for (uint64_t i = 0; i < count; ++i) {
                t_interpolationSearchKeyHeadsBigNode.insert(keys[i], keys[i]);
            }
            for (uint64_t i = 0; i < count; ++i) {
                t_interpolationSearchKeyHeadsBigNode.lookup(keys[index2[i]]);
                t_interpolationSearchKeyHeadsBigNode.remove(keys[index3[i]]);
                t_interpolationSearchKeyHeadsBigNode.insert(keys[index3[i]], keys[index4[i]]);
            }
        } 
    } else {
        std::cout << "MISSING" << std::endl;
    }
    
    std::cout << "PerformanceTestMixed END" << std::endl;
}

void runPerformanceTestLookup(vector<vector<uint8_t>> &keys,PerfEvent& perf, Btrees type) {
    std::cout << "PerformanceTestLookup START" << std::endl;
    
    std::random_shuffle(keys.begin(), keys.end());

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

    if (type == MAP) {
        TesterPerformanceMap t_map;
        // Pre-Insert all keys in Btree for Lookup Performance Test
        for (uint64_t i = 0; i < count; ++i) {
            //t_binarySearch.insert(keys[i], keys[i]);
            t_map.insert(keys[i], keys[i]);
        }
        {
            PerfEventBlock peb(perf,count,{"PerformanceTestLookup stdMap"});
            for (uint64_t i = 0; i < count; ++i) {
                t_map.lookup(keys[i]);
            }
            for (uint64_t i = 0; i < count; ++i) {
                t_map.lookup(keys[iteration2[i]]);
            }
            for (uint64_t i = 0; i < count; ++i) {
                t_map.lookup(keys[iteration3[i]]);
            }
        }
    } else if (type == TEMPLATE) {
        TesterPerformanceTempl t_templ{};
        // Pre-Insert all keys in Btree for Lookup Performance Test
        for (uint64_t i = 0; i < count; ++i) {
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
        }
    } else if (type == TEMPLATEBIGNODE) {
        TesterPerformanceTemplBigNode t_templBigNode{};
        // Pre-Insert all keys in Btree for Lookup Performance Test
        for (uint64_t i = 0; i < count; ++i) {
            //t_binarySearch.insert(keys[i], keys[i]);
            t_templBigNode.insert(keys[i], keys[i]);
        }
        {
            PerfEventBlock peb(perf,count,{"PerformanceTestLookup TemplateBigNode"});
            for (uint64_t i = 0; i < count; ++i) {
                t_templBigNode.lookup(keys[i]);
            }
            for (uint64_t i = 0; i < count; ++i) {
                t_templBigNode.lookup(keys[iteration2[i]]);
            }
            for (uint64_t i = 0; i < count; ++i) {
                t_templBigNode.lookup(keys[iteration3[i]]);
            }
        }
    } else if (type == BINARYSEARCH) {
        TesterPerformanceBinarySearch t_binarySearch{};
        // Pre-Insert all keys in Btree for Lookup Performance Test
        for (uint64_t i = 0; i < count; ++i) {
            //t_binarySearch.insert(keys[i], keys[i]);
            t_binarySearch.insert(keys[i], keys[i]);
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
    } else if (type == BINARYSEARCHBIGNODE) {
        TesterPerformanceBinarySearchBigNode t_binarySearchBigNode{};        
        // Pre-Insert all keys in Btree for Lookup Performance Test
        for (uint64_t i = 0; i < count; ++i) {
            //t_binarySearch.insert(keys[i], keys[i]);
            t_binarySearchBigNode.insert(keys[i], keys[i]);
        }
        {
            PerfEventBlock peb(perf,count,{"PerformanceTestLookup BinarySearchBigNode"});
            for (uint64_t i = 0; i < count; ++i) {
                t_binarySearchBigNode.lookup(keys[i]);
            }
            for (uint64_t i = 0; i < count; ++i) {
                t_binarySearchBigNode.lookup(keys[iteration2[i]]);
            }
            for (uint64_t i = 0; i < count; ++i) {
                t_binarySearchBigNode.lookup(keys[iteration3[i]]);
            }
        }
    } else if (type == BINARYSEARCHWITHHINTS) {
        TesterPerformanceBinarySearchHints t_binarySearchHints{};      
        // Pre-Insert all keys in Btree for Lookup Performance Test
        for (uint64_t i = 0; i < count; ++i) {
            //t_binarySearch.insert(keys[i], keys[i]);
            t_binarySearchHints.insert(keys[i], keys[i]);
        }
        {
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
        }
    } else if (type == BINARYSEARCHNOPREFIX) {
        TesterPerformanceBinarySearchNoPrefix t_binarySearchNoPrefix{};       
        // Pre-Insert all keys in Btree for Lookup Performance Test
        for (uint64_t i = 0; i < count; ++i) {
            //t_binarySearch.insert(keys[i], keys[i]);
            t_binarySearchNoPrefix.insert(keys[i], keys[i]);
        }
        {
            PerfEventBlock peb(perf,count,{"PerformanceTestLookup BinarySearch without Prefix"});
            for (uint64_t i = 0; i < count; ++i) {
                t_binarySearchNoPrefix.lookup(keys[i]);
            }
            for (uint64_t i = 0; i < count; ++i) {
                t_binarySearchNoPrefix.lookup(keys[iteration2[i]]);
            }
            for (uint64_t i = 0; i < count; ++i) {
                t_binarySearchNoPrefix.lookup(keys[iteration3[i]]);
            }
        }
    } else if (type == LINEARSEARCH) {
        TesterPerformanceLinearSearch t_linearSearch{};     
        // Pre-Insert all keys in Btree for Lookup Performance Test
        for (uint64_t i = 0; i < count; ++i) {
            //t_binarySearch.insert(keys[i], keys[i]);
            t_linearSearch.insert(keys[i], keys[i]);
        }
        {
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
        }
    } else if (type == INTERPOLATIONSEARCH) {
        TesterPerformanceInterpolationSearch t_interpolationSearch{};    
        // Pre-Insert all keys in Btree for Lookup Performance Test
        for (uint64_t i = 0; i < count; ++i) {
            //t_binarySearch.insert(keys[i], keys[i]);
            t_interpolationSearch.insert(keys[i], keys[i]);
        }
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
    } else if (type == INTERPOLATIONSEARCHBIGNODE) {
        TesterPerformanceInterpolationSearchBigNode t_interpolationSearchBigNode{};    
        // Pre-Insert all keys in Btree for Lookup Performance Test
        for (uint64_t i = 0; i < count; ++i) {
            //t_binarySearch.insert(keys[i], keys[i]);
            t_interpolationSearchBigNode.insert(keys[i], keys[i]);
        }
        {
            PerfEventBlock peb(perf,count,{"PerformanceTestLookup InterpolationSearchBigNode"});
            for (uint64_t i = 0; i < count; ++i) {
                t_interpolationSearchBigNode.lookup(keys[i]);
            }
            for (uint64_t i = 0; i < count; ++i) {
                t_interpolationSearchBigNode.lookup(keys[iteration2[i]]);
            }
            for (uint64_t i = 0; i < count; ++i) {
                t_interpolationSearchBigNode.lookup(keys[iteration3[i]]);
            }
        }
    } else if (type == INTERPOLATIONSEARCHWITHKEYHEADS) {
        TesterPerformanceInterpolationSearchKeyHeads t_interpolationSearchKeyHeads{};  
        // Pre-Insert all keys in Btree for Lookup Performance Test
        for (uint64_t i = 0; i < count; ++i) {
            //t_binarySearch.insert(keys[i], keys[i]);
            t_interpolationSearchKeyHeads.insert(keys[i], keys[i]);
        }
        {
            PerfEventBlock peb(perf,count,{"PerformanceTestLookup InterpolationSearch with KeyHeads"});
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
    } else if (type == INTERPOLATIONSEARCHWITHKEYHEADSBIGNODE) {
        TesterPerformanceInterpolationSearchKeyHeadsBigNode t_interpolationSearchKeyHeadsBigNode{};
        // Pre-Insert all keys in Btree for Lookup Performance Test
        for (uint64_t i = 0; i < count; ++i) {
            //t_binarySearch.insert(keys[i], keys[i]);
            t_interpolationSearchKeyHeadsBigNode.insert(keys[i], keys[i]);
        }
        {
            PerfEventBlock peb(perf,count,{"PerformanceTestLookup InterpolationSearch with KeyHeadsBigNode"});
            for (uint64_t i = 0; i < count; ++i) {
                t_interpolationSearchKeyHeadsBigNode.lookup(keys[i]);
            }
            for (uint64_t i = 0; i < count; ++i) {
                t_interpolationSearchKeyHeadsBigNode.lookup(keys[iteration2[i]]);
            }
            for (uint64_t i = 0; i < count; ++i) {
                t_interpolationSearchKeyHeadsBigNode.lookup(keys[iteration3[i]]);
            }
        }
    } else {
        std::cout << "MISSING" << std::endl;
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
    std::cout << std::endl;
    std::cout << std::endl;
}

std::vector<uint8_t> stringToVector(const std::string &str) {
    return std::vector<uint8_t>(str.begin(), str.end());
}

std::vector<uint32_t> fal(double shape, uint64_t size) {
        std::vector<uint32_t> v(size);
        for (uint64_t i = 0; i < v.size() - 1; i++) {
            // scale up to ensure elements are distinct for as long as possible
             v[i] = pow((double)(size - i), - shape) * std::numeric_limits<uint32_t>::max();
        }
        v.back() = std::numeric_limits<uint32_t>::max(); // Make last element max value
        return v;
}

vector<vector<uint8_t>> intToBigEndianByteVector(std::vector<uint32_t> v) {
    vector<vector<uint8_t>> data;
    for (uint64_t i = 0; i < v.size(); i++) {
        uint8_t bytes[4];
        for (int j = 0; j < 4; j++) {
            bytes[3 - j] = static_cast<uint8_t>(v[i] >> (j * 8));
        }
        data.emplace_back(bytes, bytes + 4);
    }
    return data;
}

bool keyIntLimited(Btrees type) {
    if (type == INTERPOLATIONSEARCHWITHKEYHEADS || type == INTERPOLATIONSEARCHWITHKEYHEADSBIGNODE) {
        return true;
    }
    return false;
}

int main() {
    srand(42);
    PerfEvent perf;

    Btrees type;

    if (getenv("ANALYSIS")) {
        BtreeAnalysis t{};
        std::cout << "ANALYSIS on BTree with CONTENT_SIZE = " << t.btree->root->CONTENT_SIZE << std::endl;
        type = ANALYSIS;
    } else if (getenv("TEST")) {
        Tester t{};
        std::cout << "TESTS running on BTree with CONTENT_SIZE = " << t.btree->root->CONTENT_SIZE << " (Compared with BtreeTemplate Content_Size = " << t.btreeTemplate->root->CONTENT_SIZE << ")" << std::endl;
        type = TEST;
    } else if (getenv("BINARY")) {
        if (getenv("BIGNODE")) {
            TesterPerformanceBinarySearchBigNode t{};
            std::cout << "PERFORMANCETESTS on BinaryBigNode Btree with CONTENT_SIZE = " << t.btreeBinarySearchBigNode->root->CONTENT_SIZE << std::endl;
            type = BINARYSEARCHBIGNODE;
        } else if (getenv("NOPREFIX")) {
            TesterPerformanceBinarySearchNoPrefix t{};
            std::cout << "PERFORMANCETESTS on BinarySearchNoPrefix Btree with CONTENT_SIZE = " << t.btreeBinarySearchNoPrefix->root->CONTENT_SIZE << std::endl;
            type = BINARYSEARCHNOPREFIX;
        } else if (getenv("HINTS")) {
            TesterPerformanceBinarySearchHints t{};
            std::cout << "PERFORMANCETESTS on BinarySearchHints Btree with CONTENT_SIZE = " << t.btreeBinarySearchHints->root->CONTENT_SIZE << std::endl;
            type = BINARYSEARCHWITHHINTS;
        } else {
            TesterPerformanceBinarySearch t{};
            std::cout << "PERFORMANCETESTS on BinarySearch Btree with CONTENT_SIZE = " << t.btreeBinarySearch->root->CONTENT_SIZE << std::endl;
            type = BINARYSEARCH;
        }
    } else if (getenv("INTERPOLATION")) {
        if (getenv("KEYHEADS")) {
            if (getenv("BIGNODE")) {
                TesterPerformanceInterpolationSearchKeyHeadsBigNode t{};
                std::cout << "PERFORMANCETESTS on InterpolationSearchKeyHeadsBigNode Btree with CONTENT_SIZE = " << t.btreeInterpolationSearchKeyHeadsBigNode->root->CONTENT_SIZE << std::endl;
                type = INTERPOLATIONSEARCHWITHKEYHEADSBIGNODE;
            } else {
                TesterPerformanceInterpolationSearchKeyHeads t{};
                std::cout << "PERFORMANCETESTS on InterpolationSearchKeyHeads Btree with CONTENT_SIZE = " << t.btreeInterpolationSearchKeyHeads->root->CONTENT_SIZE << std::endl;
                type = INTERPOLATIONSEARCHWITHKEYHEADS;
            }
        } else if (getenv("BIGNODE")) {
            TesterPerformanceInterpolationSearchBigNode t{};
            std::cout << "PERFORMANCETESTS on InterpolationSearchBigNode Btree with CONTENT_SIZE = " << t.btreeInterpolationSearchBigNode->root->CONTENT_SIZE << std::endl;
            type = INTERPOLATIONSEARCHBIGNODE;
        } else {
            TesterPerformanceInterpolationSearch t{};
            std::cout << "PERFORMANCETESTS on InterpolationSearch Btree with CONTENT_SIZE = " << t.btreeInterpolationSearch->root->CONTENT_SIZE << std::endl;
            type = INTERPOLATIONSEARCH;
        }
    } else if (getenv("LINEAR")) {
        if (getenv("BIGNODE")) {
            TesterPerformanceLinearSearchBigNode t{};
            std::cout << "PERFORMANCETESTS on LinearSearchBigNode Btree with CONTENT_SIZE = " << t.btreeLinearSearchBigNode->root->CONTENT_SIZE << std::endl;
            type = LINEARSEARCHBIGNODE;
        } else {
            TesterPerformanceLinearSearch t{};
            std::cout << "PERFORMANCETESTS on LinearSearch Btree with CONTENT_SIZE = " << t.btreeLinearSearch->root->CONTENT_SIZE << std::endl;
            type = LINEARSEARCH;
        }
    } else if (getenv("MAP")) {
        std::cout << "PERFORMANCETESTS on STD::MAP" << std::endl;
        type = MAP;
    } else if (getenv("AS")) {

    } else if (getenv("IBS")) {
    
    } else if (getenv("SLIP")) {
    
    } else if (getenv("TIP")) {
    
    } else {
        TesterPerformanceTempl t{};
        std::cout << "No BTree type detected for PERFORMANCETESTS! BTreeTemplate will be used with CONTENT_SIZE = " << t.btreeTemplate->root->CONTENT_SIZE << std::endl;
        type = TEMPLATE; 
    }

    // For Analysis:
    uint32_t btreeContentSize = 0;
    if (type == ANALYSIS) {
        BtreeAnalysis t{};
        btreeContentSize = t.btree->root->CONTENT_SIZE;
    }
    uint64_t numKeysFullInnerNode = btreeContentSize / 4; // Assuming all keys are 4 Byte (Ints)
    uint64_t numKeysFullLeafAverage = btreeContentSize / 8; // Assuming most value entries are also 4 byte (Int)


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
        if (type == ANALYSIS) {
            std::cout << "BTree Distribution Analysis for datatype: INTUNIFORM with " << n << " keys (Node ContentSize = " << btreeContentSize << ")" << std::endl;
            runAnalysis(data);
            std::cout << std::endl;
            
            std::cout << "Single Leaf Replication Distribution Analysis for datatype: INTUNIFORM with " << numKeysFullLeafAverage << " keys in Leaf" << std::endl;
            runSingleNodeDistributionByteKeys(data, numKeysFullLeafAverage, false);
            std::cout << std::endl;
            std::cout << "Single InnerNode Replication Distribution Analysis for datatype: INTUNIFORM with " << numKeysFullInnerNode << " keys in InnerNode" << std::endl;
            runSingleNodeDistributionByteKeys(data, numKeysFullInnerNode, false);
            std::cout << std::endl;

            std::cout << "Single Node Replication Distribution Analysis for datatype: INTUNIFORM with all " << n << " keys in 1 'Node'" << std::endl;
            runSingleNodeDistributionByteKeys(data, n, false);
        } else if (type == TEST) {
            runTest(data);
            runTestReverse(data);
            runTestZickZack(data);
            runTestMixed(data);
        } else {
            std::cout << "DATATYPE: INTUNIFORM=" << n << std::endl;
            runPerformanceTestStandard(data, perf, type);
            //runPerformanceTestMixed(data, perf, type);
            //runPerformanceTestLookup(data, perf, type);
        }
        std::cout << std::endl;
        std::cout << std::endl;
    }

    if (getenv("INTLITTLEENDIAN")) { // Fills up most significant Byte first
        vector<vector<uint8_t>> data;
        vector<uint64_t> v;
        uint64_t n = atof(getenv("INTLITTLEENDIAN"));
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
        if (type == ANALYSIS) {
            std::cout << "BTree Distribution Analysis for datatype: INTLITTLEENDIAN with " << n << " keys (Node ContentSize = " << btreeContentSize << ")" << std::endl;
            runAnalysis(data);
            std::cout << std::endl;
            
            std::cout << "Single Leaf Replication Distribution Analysis for datatype: INTLITTLEENDIAN with " << numKeysFullLeafAverage << " keys in Leaf" << std::endl;
            runSingleNodeDistributionByteKeys(data, numKeysFullLeafAverage, false);
            std::cout << std::endl;
            std::cout << "Single Leaf Replication Distribution Analysis for datatype: INTLITTLEENDIAN (SORTED for Comparison) with " << numKeysFullLeafAverage << " keys in Leaf" << std::endl;
            runSingleNodeDistributionByteKeys(data, numKeysFullLeafAverage, true);
            std::cout << "Single InnerNode Replication Distribution Analysis for datatype: INTLITTLEENDIAN with " << numKeysFullInnerNode << " keys in InnerNode" << std::endl;
            runSingleNodeDistributionByteKeys(data, numKeysFullInnerNode, false);
            std::cout << std::endl;

            std::cout << "Single Node Replication Distribution Analysis for datatype: INTLITTLEENDIAN with all " << n << " keys in 1 'Node'" << std::endl;
            runSingleNodeDistributionByteKeys(data, n, false);
        } else if (type == TEST) {
            runTest(data);
            runTestReverse(data);
            runTestZickZack(data);
            runTestMixed(data);
        } else {
            std::cout << "DATATYPE: INTLITTLEENDIAN=" << n << std::endl;
            runPerformanceTestStandard(data, perf, type);
            //runPerformanceTestMixed(data, perf, type);
            //runPerformanceTestLookup(data, perf, type);
        }
        std::cout << std::endl;
        std::cout << std::endl;
    }

    if (getenv("INTFAL")) {
        uint64_t n = atof(getenv("INTFAL"));
        double shape = atof(getenv("FALSHAPE")) / 100.0;
        std::vector<uint32_t> v = fal(shape, n);
        vector<vector<uint8_t>> data = intToBigEndianByteVector(v);
        if (type == ANALYSIS) {
            std::cout << "BTree Distribution Analysis for datatype: INTFAL with shape: " << shape << " and with " << n << " keys (Node ContentSize = " << btreeContentSize << ")" << std::endl;
            runAnalysis(data);
            std::cout << std::endl;

            std::cout << "Single Leaf Replication Distribution Analysis for datatype: INTFAL with " << numKeysFullLeafAverage << " keys in Leaf" << std::endl;
            std::vector<uint32_t> keysLeafNodeReplication = fal(shape, numKeysFullInnerNode);
            runSingleNodeDistribution(keysLeafNodeReplication, numKeysFullLeafAverage, false);
            std::cout << std::endl;

            std::cout << "Single InnerNode Replication Distribution Analysis for datatype: INTFAL with " << numKeysFullInnerNode << " keys in InnerNode" << std::endl;
            std::vector<uint32_t> keysInnerNodeReplication = fal(shape, numKeysFullInnerNode);
            runSingleNodeDistribution(keysInnerNodeReplication, numKeysFullInnerNode, false);
            std::cout << std::endl;

            std::cout << "Single Node Replication Distribution Analysis for datatype: INTFAL with all " << n << " keys in 1 'Node'" << std::endl;
            runSingleNodeDistribution(v, n, false);
        } else if (type == TEST) {
            runTest(data);
            runTestReverse(data);
            runTestZickZack(data);
            runTestMixed(data);
        } else {
            uint64_t s = atof(getenv("FALSHAPE"));
            std::cout << "DATATYPE: INTFAL=" << n << " FALSHAPE=" << s << std::endl;
            runPerformanceTestStandard(data, perf, type);
            //runPerformanceTestMixed(data, perf, type);
            //runPerformanceTestLookup(data, perf, type);
        }
        std::cout << std::endl;
        std::cout << std::endl;
    }

    if (getenv("INTCFAL")) {
        uint64_t n = atof(getenv("INTCFAL"));
        double shape = atof(getenv("CFALSHAPE")) / 100.0;
        std::vector<uint32_t> v = fal(shape, n);
        auto max_sum = std::accumulate(v.begin(), v.end(), 0.0L);
        auto scale = std::numeric_limits<uint32_t>::max() / max_sum;
        std::transform(v.begin(), v.end(), v.begin(),
                   [scale](auto x) { return x * scale; });
        std::partial_sum(v.begin(), v.end(), v.begin());
        vector<vector<uint8_t>> data = intToBigEndianByteVector(v);
        if (type == ANALYSIS) {
            std::cout << "BTree Distribution Analysis for datatype: INTCFAL with shape: " << shape << " and with " << n << " keys (Node ContentSize = " << btreeContentSize << ")" << std::endl;
            runAnalysis(data);
            std::cout << std::endl;

            std::cout << "Single Leaf Replication Distribution Analysis for datatype: INTCFAL with " << numKeysFullLeafAverage << " keys in Leaf" << std::endl;
            std::vector<uint32_t> keysLeafNodeReplication = fal(shape, numKeysFullLeafAverage);
            max_sum = std::accumulate(keysLeafNodeReplication.begin(), keysLeafNodeReplication.end(), 0.0L);
            scale = std::numeric_limits<uint32_t>::max() / max_sum;
            std::transform(keysLeafNodeReplication.begin(), keysLeafNodeReplication.end(), keysLeafNodeReplication.begin(),
                   [scale](auto x) { return x * scale; });
            std::partial_sum(keysLeafNodeReplication.begin(), keysLeafNodeReplication.end(), keysLeafNodeReplication.begin());
            runSingleNodeDistribution(keysLeafNodeReplication, numKeysFullLeafAverage, false);
            std::cout << std::endl;

            std::cout << "Single InnerNode Replication Distribution Analysis for datatype: INTCFAL with " << numKeysFullInnerNode << " keys in InnerNode" << std::endl;
            std::vector<uint32_t> keysInnerNodeReplication = fal(shape, numKeysFullInnerNode);
            max_sum = std::accumulate(keysInnerNodeReplication.begin(), keysInnerNodeReplication.end(), 0.0L);
            scale = std::numeric_limits<uint32_t>::max() / max_sum;
            std::transform(keysInnerNodeReplication.begin(), keysInnerNodeReplication.end(), keysInnerNodeReplication.begin(),
                   [scale](auto x) { return x * scale; });
            std::partial_sum(keysInnerNodeReplication.begin(), keysInnerNodeReplication.end(), keysInnerNodeReplication.begin());
            runSingleNodeDistribution(keysInnerNodeReplication, numKeysFullInnerNode, false);
            std::cout << std::endl;

            std::cout << "Single Node Replication Distribution Analysis for datatype: INTCFAL with all " << n << " keys in 1 'Node'" << std::endl;
            runSingleNodeDistribution(v, n, false);
        } else if (type == TEST) {
            runTest(data);
            runTestReverse(data);
            runTestZickZack(data);
            runTestMixed(data);
        } else {
            uint64_t s = atof(getenv("CFALSHAPE"));
            std::cout << "DATATYPE: INTCFAL=" << n << " CFALSHAPE=" << s << std::endl;
            runPerformanceTestStandard(data, perf, type);
            //runPerformanceTestMixed(data, perf, type);
            //runPerformanceTestLookup(data, perf, type);
        }
        std::cout << std::endl;
        std::cout << std::endl;
    }

    if (getenv("INTRANDOM")) {
        vector<uint32_t> randomNumbers;
        uint64_t n = atof(getenv("INTRANDOM"));

        std::random_device rd;
        std::mt19937 gen(rd());

        if (getenv("INTRANDOMSEED")) {
            auto seed = atof(getenv("INTRANDOMSEED"));
            gen.seed(seed);
        }
        std::uniform_int_distribution<uint32_t> dis(0, UINT32_MAX);
    
        for(uint64_t i = 0; i < n; i++) {
            uint32_t randomNumber = dis(gen);
            randomNumbers.push_back(randomNumber);
        }

        vector<vector<uint8_t>> data = intToBigEndianByteVector(randomNumbers);

        if (type == ANALYSIS) {
            std::cout << "BTree Distribution Analysis for datatype: INTRANDOM with " << n << " keys (Node ContentSize = " << btreeContentSize << ")" << std::endl;
            runAnalysis(data);
            std::cout << std::endl;
            
            std::cout << "Single Leaf Replication Distribution Analysis for datatype: INTRANDOM with " << numKeysFullLeafAverage << " keys in Leaf" << std::endl;
            runSingleNodeDistribution(randomNumbers, numKeysFullLeafAverage, false);
            std::cout << std::endl;
            std::cout << "Single Leaf Replication Distribution Analysis for datatype: INTRANDOM (SORTED for Comparison) with " << numKeysFullLeafAverage << " keys in Leaf" << std::endl;
            runSingleNodeDistribution(randomNumbers, numKeysFullLeafAverage, true);
            std::cout << std::endl;
            std::cout << "Single InnerNode Replication Distribution Analysis for datatype: INTRANDOMS with " << numKeysFullInnerNode << " keys in InnerNode" << std::endl;
            runSingleNodeDistribution(randomNumbers, numKeysFullInnerNode, false);
            std::cout << std::endl;

            std::cout << "Single Node Replication Distribution Analysis for datatype: INTRANDOM with all " << n << " keys in 1 'Node'" << std::endl;
            runSingleNodeDistribution(randomNumbers, n, false);
        } else if (type == TEST) {
            runTest(data);
            runTestReverse(data);
            runTestZickZack(data);
            runTestMixed(data);
        } else {
            std::cout << "DATATYPE: INTRANDOM=" << n << " INTRANDOMSEED=";
            if (getenv("INTRANDOMSEED")) {
                uint64_t seed = atof(getenv("INTRANDOMSEED"));
                std::cout << seed << std::endl;
            } else {
                std::cout << "RANDOM" << std::endl;
            }
            runPerformanceTestStandard(data, perf, type);
            //runPerformanceTestMixed(data, perf, type);
            //runPerformanceTestLookup(data, perf, type);
        }
        std::cout << std::endl;
        std::cout << std::endl;
    }

    if (getenv("INTRANDOMBYTES")) {
        vector<vector<uint8_t>> data;
        uint64_t n = atof(getenv("INTRANDOMBYTES"));
        for (uint64_t i = 0; i < n; i++) {
            uint8_t bytes[4];
            bytes[0] = rand() % 255;
            bytes[1] = rand() % 255;
            bytes[2] = rand() % 255;
            bytes[3] = rand() % 255;
            data.emplace_back(bytes, bytes + 4);
        }
        
        if (type == ANALYSIS) {
            std::cout << "BTree Distribution Analysis for datatype: INTRANDOMBYTES with " << n << " keys (Node ContentSize = " << btreeContentSize << ")" << std::endl;
            runAnalysis(data);
            std::cout << std::endl;
            
            std::cout << "Single Leaf Replication Distribution Analysis for datatype: INTRANDOMBYTES with " << numKeysFullLeafAverage << " keys in Leaf" << std::endl;
            runSingleNodeDistributionByteKeys(data, numKeysFullLeafAverage, false);
            std::cout << std::endl;
            std::cout << "Single Leaf Replication Distribution Analysis for datatype: INTRANDOMBYTES (SORTED for Comparison) with " << numKeysFullInnerNode << " keys in Leaf" << std::endl;
            runSingleNodeDistributionByteKeys(data, numKeysFullLeafAverage, true);
            std::cout << std::endl;
            std::cout << "Single InnerNode Replication Distribution Analysis for datatype: INTRANDOMBYTES with " << numKeysFullInnerNode << " keys in InnerNode" << std::endl;
            runSingleNodeDistributionByteKeys(data, numKeysFullInnerNode, false);
            std::cout << std::endl;

            std::cout << "Single Node Replication Distribution Analysis for datatype: INTRANDOMBYTES with all " << n << " keys in 1 'Node'" << std::endl;
            runSingleNodeDistributionByteKeys(data, n, false);
        } else if (type == TEST) {
            runTest(data);
            runTestReverse(data);
            runTestZickZack(data);
            runTestMixed(data);
        } else {
            std::cout << "DATATYPE: INTRANDOMBYTES=" << n << std::endl;
            runPerformanceTestStandard(data, perf, type);
            //runPerformanceTestMixed(data, perf, type);
            //runPerformanceTestLookup(data, perf, type);
        }
        std::cout << std::endl;
        std::cout << std::endl;
    }

    if (getenv("BYTE")) {
        vector<vector<uint8_t>> data;
        uint64_t n = atof(getenv("BYTE"));
        uint16_t byteSize = 4; // Default ByteSize
        if (getenv("BYTESIZE")) {
            byteSize = atof(getenv("BYTESIZE"));
        }
        
        if (byteSize > 4) {
            if (keyIntLimited(type)) {
                std::cout << "Selected Key Byte Size is too big for selected Btree (" << byteSize << ")! Key Byte Size reduced to 4 Byte." << std::endl;
                byteSize = 4;
            }
        }

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
        
        if (type == ANALYSIS) {
            std::cout << "BTree Distribution Analysis for datatype: BYTE with byteSize: " << byteSize << " and with " << n << " keys (Node ContentSize = " << btreeContentSize << ")" << std::endl;
            runAnalysis(data);
            std::cout << std::endl;
            
            std::cout << "Single Leaf Replication Distribution Analysis for datatype: BYTE with " << numKeysFullLeafAverage << " keys in Leaf" << std::endl;
            runSingleNodeDistributionByteKeys(data, numKeysFullLeafAverage, false);
            std::cout << std::endl;
            std::cout << "Single InnerNode Replication Distribution Analysis for datatype: BYTE with " << numKeysFullInnerNode << " keys in InnerNode" << std::endl;
            runSingleNodeDistributionByteKeys(data, numKeysFullInnerNode, false);
            std::cout << std::endl;

            std::cout << "Single Node Replication Distribution Analysis for datatype: BYTE with all " << n << " keys in 1 'Node'" << std::endl;
            runSingleNodeDistributionByteKeys(data, n, false);
        } else if (type == TEST) {
            runTest(data);
            runTestReverse(data);
            runTestZickZack(data);
            runTestMixed(data);
        } else {
            std::cout << "DATATYPE: BYTE=" << n << "BYTESIZE=" << byteSize << std::endl;
            runPerformanceTestStandard(data, perf, type);
            //runPerformanceTestMixed(data, perf, type);
            //runPerformanceTestLookup(data, perf, type);
        }
        std::cout << std::endl;
        std::cout << std::endl;
    }

    if (getenv("VARIABLEBYTE")) {
        vector<vector<uint8_t>> data;
        uint64_t n = atof(getenv("VARIABLEBYTE"));
        uint16_t keyMaxByteSize = 250;
        uint16_t bigkeyMaxByteSize = 1000;
        uint16_t bigKeyByteSizeIterations = 0; // Number of iterations for keys over keyMaxByteSize (= 500Byte) to focus mostly on keys under 500 byte
        vector<uint16_t> keyByteSizes;
        
        if (keyIntLimited(type)) {
            bigKeyByteSizeIterations = 0;
            keyMaxByteSize = 4;
            std::cout << "KeyMaxByteSize for VARIABLEBYTE reduced to 4 Byte." << std::endl;
        }

        for (int j = 0; j < bigKeyByteSizeIterations; j++) {
            for (uint16_t i = 0; i <= bigkeyMaxByteSize && i < n; i++) {
                keyByteSizes.push_back(i);
            }
        }
        while(keyByteSizes.size() < n) {
            for (uint16_t i = 0; i <= keyMaxByteSize && keyByteSizes.size() < n; i++) {
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
        
        if (type == ANALYSIS) {
            std::cout << "BTree Distribution Analysis for datatype: VARIABLEBYTE with keyMaxByteSize: " << keyMaxByteSize << " and with " << n << " keys (Node ContentSize = " << btreeContentSize << ")" << std::endl;
            runAnalysis(data);
            std::cout << std::endl;
            
            std::cout << "Single Leaf Replication Distribution Analysis for datatype: VARIABLEBYTE with " << numKeysFullLeafAverage << " keys in Leaf" << std::endl;
            runSingleNodeDistributionByteKeys(data, numKeysFullLeafAverage, false);
            std::cout << std::endl;
            std::cout << "Single InnerNode Replication Distribution Analysis for datatype: VARIABLEBYTE with " << numKeysFullInnerNode << " keys in InnerNode" << std::endl;
            runSingleNodeDistributionByteKeys(data, numKeysFullInnerNode, false);
            std::cout << std::endl;

            std::cout << "Single Node Replication Distribution Analysis for datatype: VARIABLEBYTE with all " << n << " keys in 1 'Node'" << std::endl;
            runSingleNodeDistributionByteKeys(data, n, false);
        } else if (type == TEST) {
            runTest(data);
            runTestReverse(data);
            runTestZickZack(data);
            runTestMixed(data);
        } else {
            std::cout << "DATATYPE: VARIABLEBYTE=" << n << "keyMaxByteSize=" << keyMaxByteSize << std::endl;
            runPerformanceTestStandard(data, perf, type);
            //runPerformanceTestMixed(data, perf, type);
            //runPerformanceTestLookup(data, perf, type);
        }
        std::cout << std::endl;
        std::cout << std::endl;
    }

    if (getenv("LONG1")) {
        if (keyIntLimited(type)) {
            std::cout << "Btree restricted to Int Keys. LONG1 test canceled." << std::endl;
        } else {
            vector<vector<uint8_t>> data;
            uint64_t n = atof(getenv("LONG1"));
            for (unsigned i = 0; i < n; i++) {
                string s;
                for (unsigned j = 0; j < i; j++)
                    s.push_back('A');
                data.push_back(stringToVector(s));;
            }
        
            if (type == ANALYSIS) {
            std::cout << "BTree Distribution Analysis for datatype: LONG1 with " << n << " keys (Node ContentSize = " << btreeContentSize << ")" << std::endl;
                runAnalysis(data);
                std::cout << std::endl;
            
                std::cout << "Single Leaf Replication Distribution Analysis for datatype: LONG1 with " << numKeysFullLeafAverage << " keys in Leaf" << std::endl;
                runSingleNodeDistributionByteKeys(data, numKeysFullLeafAverage, false);
                std::cout << std::endl;
                std::cout << "Single InnerNode Replication Distribution Analysis for datatype: LONG1 with " << numKeysFullInnerNode << " keys in InnerNode" << std::endl;
                runSingleNodeDistributionByteKeys(data, numKeysFullInnerNode, false);
                std::cout << std::endl;

                std::cout << "Single Node Replication Distribution Analysis for datatype: LONG1 with all " << n << " keys in 1 'Node'" << std::endl;
                runSingleNodeDistributionByteKeys(data, n, false);
            } else if (type == TEST) {
                runTest(data);
                runTestReverse(data);
                runTestZickZack(data);
                runTestMixed(data);
            } else {
                std::cout << "DATATYPE: LONG1=" << n << std::endl;
                runPerformanceTestStandard(data, perf, type);
                //runPerformanceTestMixed(data, perf, type);
                //runPerformanceTestLookup(data, perf, type);
            }
        }
        std::cout << std::endl;
        std::cout << std::endl;
    }

    if (getenv("LONG2")) {
        if (keyIntLimited(type)) {
            std::cout << "Btree restricted to Int Keys. LONG2 test canceled." << std::endl;
        } else {
            vector<vector<uint8_t>> data;
            uint64_t n = atof(getenv("LONG2"));
            for (unsigned i = 0; i < n; i++) {
                string s;
                for (unsigned j = 0; j < i; j++)
                    s.push_back('A' + random() % 60);
                data.push_back(stringToVector(s));
            }
        
            if (type == ANALYSIS) {
                std::cout << "BTree Distribution Analysis for datatype: LONG2 with " << n << " keys (Node ContentSize = " << btreeContentSize << ")" << std::endl;
                runAnalysis(data);
                std::cout << std::endl;
            
                std::cout << "Single Leaf Replication Distribution Analysis for datatype: LONG2 with " << numKeysFullLeafAverage << " keys in Leaf" << std::endl;
                runSingleNodeDistributionByteKeys(data, numKeysFullLeafAverage, false);
                std::cout << std::endl;
                std::cout << "Single InnerNode Replication Distribution Analysis for datatype: LONG2 with " << numKeysFullInnerNode << " keys in InnerNode" << std::endl;
                runSingleNodeDistributionByteKeys(data, numKeysFullInnerNode, false);
                std::cout << std::endl;

                std::cout << "Single Node Replication Distribution Analysis for datatype: LONG2 with all " << n << " keys in 1 'Node'" << std::endl;
                runSingleNodeDistributionByteKeys(data, n, false);
            } else if (type == TEST) {
                runTest(data);
                runTestMixed(data);
            } else {
                std::cout << "DATATYPE: LONG2=" << n << std::endl;
                runPerformanceTestStandard(data, perf, type);
                //runPerformanceTestMixed(data, perf, type);
                //runPerformanceTestLookup(data, perf, type);
            }
        }
        std::cout << std::endl;
        std::cout << std::endl;
    }

    if (getenv("FILE")) {
        if (keyIntLimited(type)) {
            std::cout << "Btree restricted to Int Keys. FILE test canceled." << std::endl;
        } else {
            vector<vector<uint8_t>> data;
            ifstream in(getenv("FILE"));
            string line;
            while (getline(in, line))
                data.push_back(stringToVector(line));;
        
            if (type == ANALYSIS) {
                std::cout << "BTree Distribution Analysis for datatype: FILE with " << data.size() << " keys (Node ContentSize = " << btreeContentSize << ")" << std::endl;
                std::vector<uint32_t> urlsSizes;
                for (uint64_t i = 0; i < data.size(); i++) {
                    urlsSizes.push_back(static_cast<uint8_t>(data.at(i).size()));
                }
                BtreeAnalysis t{};
                double urlsSizesMean = t.mean(urlsSizes);
                std::sort(urlsSizes.begin(), urlsSizes.end());
                double urlsSizesMedian = t.median(urlsSizes);
                uint32_t lowesturlSize = urlsSizes.at(0);
                uint32_t highesturlSize = urlsSizes.at(urlsSizes.size()-1);
                std::cout << "Urls Mean Byte-Size: " << urlsSizesMean << ", Median: " << urlsSizesMedian << ", lowestSize: " << lowesturlSize << ", highestSize: " << highesturlSize << std::endl;
                runAnalysis(data);
                std::cout << std::endl;
            
                std::cout << "Single Leaf Replication Distribution Analysis for datatype: FILE with " << numKeysFullLeafAverage << " keys in Leaf" << std::endl;
                runSingleNodeDistributionByteKeys(data, numKeysFullLeafAverage, false);
                std::cout << std::endl;
                std::cout << "Single InnerNode Replication Distribution Analysis for datatype: FILE with " << numKeysFullInnerNode << " keys in InnerNode" << std::endl;
                runSingleNodeDistributionByteKeys(data, numKeysFullInnerNode, false);
                std::cout << std::endl;

                std::cout << "Single Node Replication Distribution Analysis for datatype: FILE with all " << data.size() << " keys in 1 'Node'" << std::endl;
                runSingleNodeDistributionByteKeys(data, data.size(), false);
            } else if (type == TEST) {
                runTest(data);
                runTestMixed(data);
            } else {
                std::cout << "DATATYPE: FILE=" << data.size() << std::endl;
                runPerformanceTestStandard(data, perf, type);
                //runPerformanceTestMixed(data, perf, type);
                //runPerformanceTestLookup(data, perf, type);
            }
        }
        std::cout << std::endl;
        std::cout << std::endl;
    }
    return 0;
}