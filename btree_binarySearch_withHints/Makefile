all: btree_binarySearchHints.a

btree_binarySearchHints.a: btree_binarySearchHints.o btreenode_binarySearchHints.o utils_binarySearchHints.o
	rm -f btree_binarySearchHints.a
	ar rcs btree_binarySearchHints.a btree_binarySearchHints.o btreenode_binarySearchHints.o utils_binarySearchHints.o

btree-optimized_binarySearchHints.a: btree-optimized_binarySearchHints.o btreenode-optimized_binarySearchHints.o utils-optimized_binarySearchHints.o
	rm -f btree-optimized_binarySearchHints.a
	ar rcs btree-optimized_binarySearchHints.a btree-optimized_binarySearchHints.o btreenode-optimized_binarySearchHints.o utils-optimized_binarySearchHints.o

btree_binarySearchHints.o: btree_binarySearchHints.cpp btree_binarySearchHints.hpp
	g++ -std=c++20 -Wall -Wextra -g -c btree_binarySearchHints.cpp -o $@

btreenode_binarySearchHints.o: btreenode_binarySearchHints.cpp btreenode_binarySearchHints.hpp
	g++ -std=c++20 -Wall -Wextra -g -c btreenode_binarySearchHints.cpp -o $@

btree-optimized_binarySearchHints.o: btree_binarySearchHints.cpp btree_binarySearchHints.hpp 
	clang++ -std=c++20 -Wall -Wextra -g -c btree_binarySearchHints.cpp -o $@ -O3 -DNDEBUG

btreenode-optimized_binarySearchHints.o: btreenode_binarySearchHints.cpp btreenode_binarySearchHints.hpp
	clang++ -std=c++20 -Wall -Wextra -g -c btreenode_binarySearchHints.cpp -o $@ -O3 -DNDEBUG

utils_binarySearchHints.o: utils.hpp
	g++ -std=c++20 -Wall -Wextra -g -c utils.hpp -o $@

utils-optimized_binarySearchHints.o: utils.hpp
	clang++ -std=c++20 -Wall -Wextra -g -c utils.hpp -o $@

clean:
	rm -f btree_binarySearchHints.o btree_binarySearchHints.a btree-optimized_binarySearchHints.o btree-optimized_binarySearchHints.a btreenode_binarySearchHints.o btreenode-optimized_binarySearchHints.o utils_binarySearchHints.o utils-optimized_binarySearchHints.o