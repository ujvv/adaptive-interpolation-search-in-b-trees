all: btree_binarySearchNoPrefix.a

btree_binarySearchNoPrefix.a: btree_binarySearchNoPrefix.o btreenode_binarySearchNoPrefix.o utils_binarySearchNoPrefix.o
	rm -f btree_binarySearchNoPrefix.a
	ar rcs btree_binarySearchNoPrefix.a btree_binarySearchNoPrefix.o btreenode_binarySearchNoPrefix.o utils_binarySearchNoPrefix.o

btree-optimized_binarySearchNoPrefix.a: btree-optimized_binarySearchNoPrefix.o btreenode-optimized_binarySearchNoPrefix.o utils-optimized_binarySearchNoPrefix.o
	rm -f btree-optimized_binarySearchNoPrefix.a
	ar rcs btree-optimized_binarySearchNoPrefix.a btree-optimized_binarySearchNoPrefix.o btreenode-optimized_binarySearchNoPrefix.o utils-optimized_binarySearchNoPrefix.o

btree_binarySearchNoPrefix.o: btree_binarySearchNoPrefix.cpp btree_binarySearchNoPrefix.hpp
	g++ -std=c++20 -Wall -Wextra -g -c btree_binarySearchNoPrefix.cpp -o $@

btreenode_binarySearchNoPrefix.o: btreenode_binarySearchNoPrefix.cpp btreenode_binarySearchNoPrefix.hpp
	g++ -std=c++20 -Wall -Wextra -g -c btreenode_binarySearchNoPrefix.cpp -o $@

btree-optimized_binarySearchNoPrefix.o: btree_binarySearchNoPrefix.cpp btree_binarySearchNoPrefix.hpp 
	clang++ -std=c++20 -Wall -Wextra -g -c btree_binarySearchNoPrefix.cpp -o $@ -O3 -DNDEBUG

btreenode-optimized_binarySearchNoPrefix.o: btreenode_binarySearchNoPrefix.cpp btreenode_binarySearchNoPrefix.hpp
	clang++ -std=c++20 -Wall -Wextra -g -c btreenode_binarySearchNoPrefix.cpp -o $@ -O3 -DNDEBUG

utils_binarySearchNoPrefix.o: utils.hpp
	g++ -std=c++20 -Wall -Wextra -g -c utils.hpp -o $@

utils-optimized_binarySearchNoPrefix.o: utils.hpp
	clang++ -std=c++20 -Wall -Wextra -g -c utils.hpp -o $@

clean:
	rm -f btree_binarySearchNoPrefix.o btree_binarySearchNoPrefix.a btree-optimized_binarySearchNoPrefix.o btree-optimized_binarySearchNoPrefix.a btreenode_binarySearchNoPrefix.o btreenode-optimized_binarySearchNoPrefix.o utils_binarySearchNoPrefix.o utils-optimized_binarySearchNoPrefix.o