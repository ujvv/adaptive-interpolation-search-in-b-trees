all: btree_binarySearch.a

btree_binarySearch.a: btree_binarySearch.o btreenode_binarySearch.o utils_binarySearch.o
	rm -f btree_binarySearch.a
	ar rcs btree_binarySearch.a btree_binarySearch.o btreenode_binarySearch.o utils_binarySearch.o

btree-optimized_binarySearch.a: btree-optimized_binarySearch.o btreenode-optimized_binarySearch.o utils-optimized_binarySearch.o
	rm -f btree-optimized_binarySearch.a
	ar rcs btree-optimized_binarySearch.a btree-optimized_binarySearch.o btreenode-optimized_binarySearch.o utils-optimized_binarySearch.o

btree_binarySearch.o: btree_binarySearch.cpp btree_binarySearch.hpp
	g++ -std=c++20 -Wall -Wextra -g -c btree_binarySearch.cpp -o $@

btreenode_binarySearch.o: btreenode_binarySearch.cpp btreenode_binarySearch.hpp
	g++ -std=c++20 -Wall -Wextra -g -c btreenode_binarySearch.cpp -o $@

btree-optimized_binarySearch.o: btree_binarySearch.cpp btree_binarySearch.hpp 
	clang++ -std=c++20 -Wall -Wextra -g -c btree_binarySearch.cpp -o $@ -O3 -DNDEBUG

btreenode-optimized_binarySearch.o: btreenode_binarySearch.cpp btreenode_binarySearch.hpp
	clang++ -std=c++20 -Wall -Wextra -g -c btreenode_binarySearch.cpp -o $@ -O3 -DNDEBUG

utils_binarySearch.o: utils.hpp
	g++ -std=c++20 -Wall -Wextra -g -c utils.hpp -o $@

utils-optimized_binarySearch.o: utils.hpp
	clang++ -std=c++20 -Wall -Wextra -g -c utils.hpp -o $@

clean:
	rm -f btree_binarySearch.o btree_binarySearch.a btree-optimized_binarySearch.o btree-optimized_binarySearch.a btreenode_binarySearch.o btreenode-optimized_binarySearch.o utils_binarySearch.o utils-optimized_binarySearch.o