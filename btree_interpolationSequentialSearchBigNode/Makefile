all: btree_interpolationSequentialSearchBigNode.a

btree_interpolationSequentialSearchBigNode.a: btree_interpolationSequentialSearchBigNode.o btreenode_interpolationSequentialSearchBigNode.o utils_interpolationSequentialSearchBigNode.o
	rm -f btree_interpolationSequentialSearchBigNode.a
	ar rcs btree_interpolationSequentialSearchBigNode.a btree_interpolationSequentialSearchBigNode.o btreenode_interpolationSequentialSearchBigNode.o utils_interpolationSequentialSearchBigNode.o

btree-optimized_interpolationSequentialSearchBigNode.a: btree-optimized_interpolationSequentialSearchBigNode.o btreenode-optimized_interpolationSequentialSearchBigNode.o utils-optimized_interpolationSequentialSearchBigNode.o
	rm -f btree-optimized_interpolationSequentialSearchBigNode.a
	ar rcs btree-optimized_interpolationSequentialSearchBigNode.a btree-optimized_interpolationSequentialSearchBigNode.o btreenode-optimized_interpolationSequentialSearchBigNode.o utils-optimized_interpolationSequentialSearchBigNode.o

btree_interpolationSequentialSearchBigNode.o: btree_interpolationSequentialSearchBigNode.cpp btree_interpolationSequentialSearchBigNode.hpp
	g++ -std=c++20 -Wall -Wextra -g -c btree_interpolationSequentialSearchBigNode.cpp -o $@

btreenode_interpolationSequentialSearchBigNode.o: btreenode_interpolationSequentialSearchBigNode.cpp btreenode_interpolationSequentialSearchBigNode.hpp
	g++ -std=c++20 -Wall -Wextra -g -c btreenode_interpolationSequentialSearchBigNode.cpp -o $@

btree-optimized_interpolationSequentialSearchBigNode.o: btree_interpolationSequentialSearchBigNode.cpp btree_interpolationSequentialSearchBigNode.hpp 
	g++ -std=c++20 -Wall -Wextra -g -c btree_interpolationSequentialSearchBigNode.cpp -o $@ -O3 -DNDEBUG

btreenode-optimized_interpolationSequentialSearchBigNode.o: btreenode_interpolationSequentialSearchBigNode.cpp btreenode_interpolationSequentialSearchBigNode.hpp
	g++ -std=c++20 -Wall -Wextra -g -c btreenode_interpolationSequentialSearchBigNode.cpp -o $@ -O3 -DNDEBUG

utils_interpolationSequentialSearchBigNode.o: utils.hpp
	g++ -std=c++20 -Wall -Wextra -g -c utils.hpp -o $@

utils-optimized_interpolationSequentialSearchBigNode.o: utils.hpp
	g++ -std=c++20 -Wall -Wextra -g -c utils.hpp -o $@

clean:
	rm -f btree_interpolationSequentialSearchBigNode.o btree_interpolationSequentialSearchBigNode.a btree-optimized_interpolationSequentialSearchBigNode.o btree-optimized_interpolationSequentialSearchBigNode.a btreenode_interpolationSequentialSearchBigNode.o btreenode-optimized_interpolationSequentialSearchBigNode.o utils_interpolationSequentialSearchBigNode.o utils-optimized_interpolationSequentialSearchBigNode.o