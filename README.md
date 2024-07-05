# Adaptive Interpolation Search in B-Trees


This repository contains the source code and benchmarks for the bachelor thesis titled "Adaptive Interpolation Search Algorithms in B-Trees." The project includes implementations of various search algorithms within B-Trees, and it provides a framework for running benchmarks to compare their performance, analyzing B-tree nodes in various datasets, and testing B-tree implementations.

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

* btree_binarySearch: Implements binary search.
    * btree_binarySearch_noPrefix: Binary search without prefix optimization.
    * btree_binarySearch_withHints: Binary search with hints optimization.
* btree_interpolatedBinarySearch: Interpolated binary search.
* btree_interpolationSearch: Implements interpolation search with variable sized keys.
* btree_interpolationSearch_withKeyHeads: Interpolation search with integer keys.
* btree_interpolationSequentialSearch: Sequential interpolation search.
* btree_linearSearch: Implements linear search.
* btree_slopeReuseInterpolationSearch: Slope reuse interpolation search.
* btree_template: Template for the B-tree implementations
* btree_threePointInterpolationSearch: Three-point interpolation search

Additional btree folders exist for bigger node sizes while benchmarking the same btree implementation.

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
Only 1 Btree but multiple dataset can be benchmarked and analyzed in each run. Datasetsize is set with the environment variable value ("DATASET"=datsetsize)

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