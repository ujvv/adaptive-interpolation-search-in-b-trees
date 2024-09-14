# Adaptive Interpolation Search in B-Trees


This repository contains the source code and benchmarks for my bachelor's thesis titled "Adaptive Interpolation Search Algorithms in B-Trees." The project includes implementations of various search algorithms within B-Trees, and it provides a framework for running benchmarks to compare their performance, analyzing B-tree nodes in various datasets, and testing B-tree implementations.

## Project Structure

The repository is organized as follows:

- Benchmark_Documentation/: Documentation of all benchmarks conducted for the thesis.
- btree_*Search: Different B-Tree implementations with various search algorithms.
- data/: Stores datasets.
- Makefile: Makefile for building the project.
- PerfEvent.hpp: Header for performance event handling.
- test_main.cpp: Main file for running tests and benchmarks.

## B-Tree Implementations

Each B-Tree implementation is located in its respective folder and includes different search algorithms:

* btree_adaptiveSearch: Adaptive Search (AS) [[1]](#1).
* btree_binarySearch: Implements Binary Search (with prefix optimization).
    * btree_binarySearch_noPrefix: Binary Search without prefix optimization.
    * btree_binarySearch_withHints: Binary Search with hints optimization.
* btree_interpolatedBinarySearch: Interpolated Binary Search (IBS) [[2]](#2).
* btree_interpolationSearch: Implements Interpolation Search with variable sized keys.
* btree_interpolationSearch_withKeyHeads: Interpolation Search with integer keys.
* btree_interpolationSequentialSearch: Sequential Interpolation Search.
* btree_linearSearch: Implements Linear Search.
* btree_slopeReuseInterpolationSearch: Slope reuse interpolation search (SIP) [[3]](#3).
* btree_template: Template for the B-tree implementations
* btree_threePointInterpolationSearch: Three-point Interpolation Search (TIP) [[3]](#3)

Additional btree folders exist for bigger node sizes while benchmarking the same btree implementation.

### References
<a id="1">[1]</a> 
B. Bonasera, E. Ferrara, G. Fiumara, F. Pagano, and A. Provetti, “Adaptive search
over sorted sets,” Journal of Discrete Algorithms, vol. 30, pp. 128–133, Jan. 1, 2015,
issn: 1570-8667. doi: 10.1016/j.jda.2014.12.007. [Online]. Available: https:
//www.sciencedirect.com/science/article/pii/S1570866714001002 (visited
on 06/08/2024).

<a id="2">[2]</a> 
A. S. Mohammed, ¸S. E. Amrahov, and F. V. Çelebi, “Interpolated binary search:
An efficient hybrid search algorithm on ordered datasets,” Engineering Science
and Technology, an International Journal, vol. 24, no. 5, pp. 1072–1079, Oct. 2021,
issn: 22150986. doi: 10.1016/j.jestch.2021.02.009. [Online]. Available: https:
/ / linkinghub . elsevier . com / retrieve / pii / S221509862100046X (visited on
06/08/2024).

<a id="3">[3]</a> 
P. V. Sandt, Y. Chronis, and J. M. Patel, “Efficiently searching in-memory sorted
arrays: Revenge of the interpolation search?” In Proceedings of the 2019 International
Conference on Management of Data, Amsterdam Netherlands: ACM, Jun. 25, 2019,
pp. 36–53, isbn: 978-1-4503-5643-5. doi: 10.1145/3299869.3300075. [Online].
Available: https://dl.acm.org/doi/10.1145/3299869.3300075 (visited on
06/07/2024). (GitHub Repository: [github.com/UWHustle/Efficiently-Searching-In-Memory-Sorted-Arrays/tree/master](https://github.com/ujvv/adaptive-interpolation-search-in-b-trees/))


## Running Benchmarks

To run the benchmarks, you can use the example shell scripts (ISS.sh, perfTest.sh) and set environment variables to select the specific B-Tree implementation and test type. The main entry point for running tests and benchmarks is test_main.cpp.

### Environment Variables

* "ANALYSIS": Run analysis mode
* "TEST": Run test mode
* "BINARY": Run benchmarks on binary search B-Tree
    * "NOPREFIX": Adding this variable removes the prefix optimization from Binary Search
    * "HINTS": Adding this variable adds the hints optimization to Binary Search

* "INTERPOLATION": Run benchmarks on interpolation search B-Tree.
    * "KEYHEADS":  Adding KEYHEADS variable to INTERPOLATION leads to adding key head optimization only accepting interger keys
* "LINEAR": Run benchmarks on linear search B-Tree.
* "MAP": Run benchmarks on std::map.
* "AS": Run benchmarks on adaptive search B-Tree.
* "IBS": Run benchmarks on interpolated binary search B-Tree.
* "SIP": Run benchmarks on slope reuse interpolation search B-Tree.
* "TIP": Run benchmarks on three-point interpolation search B-Tree.
* "ISS": Run benchmarks on interpolation sequential search B-Tree.

#### B-Tree Node Size
* "BIGNODE": Variable needs to be added for benchmarking B-Trees over 65KB node sizes. Node Sizes need to be manually changed in the respective B-tree btreenode_*.hpp file

#### Datasets
Only 1 Btree but multiple dataset can be benchmarked and analyzed in each run. Dataset size is set with the environment variable value ("DATASET"=datsetsize)

* "INTUNIFORM": Uniformly distributed integers
* "INTLITTLEENDIAN": Little-endian ordered integers
* "INTFAL":  Zipf-distributed integers
    * "FALSHAPE": Shape parameter for the FAL dataset (Note: Use whole numbers, e.g., 50 instead of 0.5)
* "INTCFAL": Cumulative Zipf-distributed integers
    * "CFALSHAPE": Shape parameter for the CFAL dataset (Note: Use whole numbers, e.g., 105 instead of 1.05)
* "INTRANDOM": Randomly distributed integers
    * "INTRANDOMSEED": Seed for the random number generator
* "INTRANDOMBYTES": Randomly distributed 4-byte sequences
* "BYTE":  Fixed-size byte sequences (random bytes)
    * "BYTESIZE": Size of each byte sequence
* "VARIABLEBYTE": Variable-size byte sequences (currently limited to size 250)
* "LONG1":  Long strings of incremental characters
* "LONG2": Long strings of random characters
* "FILE": Data read from a file in data folder

### Example Usages

Runs benchmarks on Binary Search with Hints optimizaition with uniformly distributed integer dataset with 2e7 entries and random integer dataset with 1e7 entries
```
BINARY=1 HINTS=1 INTUNIFORM=2e7 INTRANDOM=1e7 ./main-optimized 
```
Runs Analysis on B-tree nodes with the fal dataset and shape parameter 0.5
```
ANALYSIS=1 INTFAL=1e7 FALSHAPE=50 ./main
```
Runs tests on the ISS B-tree implementation with help of the intrandom dataset
```
TEST=1 ISS=1 INTRANDOM=2e7 ./main
```
