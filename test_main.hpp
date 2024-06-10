#ifndef ADAPTIVE_INTERPOLATION_SEARCH_IN_B_TREES_TEST_MAIN_HPP
#define ADAPTIVE_INTERPOLATION_SEARCH_IN_B_TREES_TEST_MAIN_HPP

#include "PerfEvent.hpp"

#include <algorithm>
#include <csignal>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>
#include <numeric>

enum Btrees { ANALYSIS, TEST, BINARYSEARCH, BINARYSEARCHBIGNODE, BINARYSEARCHNOPREFIX, BINARYSEARCHWITHHINTS, INTERPOLATIONSEARCH, INTERPOLATIONSEARCHBIGNODE, INTERPOLATIONSEARCHWITHKEYHEADS, INTERPOLATIONSEARCHWITHKEYHEADSBIGNODE, ADAPTIVESEARCH, IBS, SIP, TIP, LINEARSEARCH, LINEARSEARCHBIGNODE, PLAIN, TEMPLATE, TEMPLATEBIGNODE, MAP};

void runSingleNodeDistributionByteKeys(std::vector<std::vector<uint8_t>> &keys, uint64_t count, bool sort);
void runSingleNodeDistribution(std::vector<uint32_t> &keys, uint64_t count, bool sort);
void runAnalysis(std::vector<std::vector<uint8_t>> &keys);


void runTest(std::vector<std::vector<uint8_t>> &keys);
void runTestZickZack(std::vector<std::vector<uint8_t>> &keys);
void runTestMixed(std::vector<std::vector<uint8_t>> &keys);

void runPerformanceTestStandard(std::vector<std::vector<uint8_t>> &keys, PerfEvent& perf, Btrees type);
void runPerformanceTestMixed(std::vector<std::vector<uint8_t>> &keys, PerfEvent& perf, Btrees type);
void runPerformanceTestLookup(std::vector<std::vector<uint8_t>> &keys, PerfEvent& perf, Btrees type);

void verifyBTreeNodeSizes();

std::vector<uint8_t> stringToVector(const std::string &str);
std::vector<uint32_t> fal(double shape, uint64_t size);
std::vector<std::vector<uint8_t>> intToBigEndianByteVector(std::vector<uint32_t> v);
bool keyIntLimited(Btrees type);

#endif // ADAPTIVE_INTERPOLATION_SEARCH_IN_B_TREES_TEST_MAIN_HPP