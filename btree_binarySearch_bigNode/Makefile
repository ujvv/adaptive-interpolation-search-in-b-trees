all: btree_binarySearchBig.a

btree_binarySearchBig.a: btree_binarySearchBig.o btreenode_binarySearchBig.o utils_binarySearchBig.o
	rm -f btree_binarySearchBig.a
	ar rcs btree_binarySearchBig.a btree_binarySearchBig.o btreenode_binarySearchBig.o utils_binarySearchBig.o

btree-optimized_binarySearchBig.a: btree-optimized_binarySearchBig.o btreenode-optimized_binarySearchBig.o utils-optimized_binarySearchBig.o
	rm -f btree-optimized_binarySearchBig.a
	ar rcs btree-optimized_binarySearchBig.a btree-optimized_binarySearchBig.o btreenode-optimized_binarySearchBig.o utils-optimized_binarySearchBig.o

btree_binarySearchBig.o: btree_binarySearchBigNode.cpp btree_binarySearchBigNode.hpp
	g++ -std=c++20 -Wall -Wextra -g -c btree_binarySearchBigNode.cpp -o $@

btreenode_binarySearchBig.o: btreenode_binarySearchBigNode.cpp btreenode_binarySearchBigNode.hpp
	g++ -std=c++20 -Wall -Wextra -g -c btreenode_binarySearchBigNode.cpp -o $@

btree-optimized_binarySearchBig.o: btree_binarySearchBigNode.cpp btree_binarySearchBigNode.hpp 
	clang++ -std=c++20 -Wall -Wextra -g -c btree_binarySearchBigNode.cpp -o $@ -O3 -DNDEBUG

btreenode-optimized_binarySearchBig.o: btreenode_binarySearchBigNode.cpp btreenode_binarySearchBigNode.hpp
	clang++ -std=c++20 -Wall -Wextra -g -c btreenode_binarySearchBigNode.cpp -o $@ -O3 -DNDEBUG

utils_binarySearchBig.o: utils.hpp
	g++ -std=c++20 -Wall -Wextra -g -c utils.hpp -o $@

utils-optimized_binarySearchBig.o: utils.hpp
	clang++ -std=c++20 -Wall -Wextra -g -c utils.hpp -o $@

clean:
	rm -f btree_binarySearchBig.o btree_binarySearchBig.a btree-optimized_binarySearchBig.o btree-optimized_binarySearchBig.a btreenode_binarySearchBig.o btreenode-optimized_binarySearchBig.o utils_binarySearchBig.o utils-optimized_binarySearchBig.o