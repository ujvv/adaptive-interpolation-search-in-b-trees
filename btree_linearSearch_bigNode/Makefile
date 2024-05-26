all: btree_linearSearchBig.a

btree_linearSearchBig.a: btree_linearSearchBig.o btreenode_linearSearchBig.o utils_linearSearchBig.o
	rm -f btree_linearSearchBig.a
	ar rcs btree_linearSearchBig.a btree_linearSearchBig.o btreenode_linearSearchBig.o utils_linearSearchBig.o

btree-optimized_linearSearchBig.a: btree-optimized_linearSearchBig.o btreenode-optimized_linearSearchBig.o utils-optimized_linearSearchBig.o
	rm -f btree-optimized_linearSearchBig.a
	ar rcs btree-optimized_linearSearchBig.a btree-optimized_linearSearchBig.o btreenode-optimized_linearSearchBig.o utils-optimized_linearSearchBig.o

btree_linearSearchBig.o: btree_linearSearchBigNode.cpp btree_linearSearchBigNode.hpp
	g++ -std=c++20 -Wall -Wextra -g -c btree_linearSearchBigNode.cpp -o $@

btreenode_linearSearchBig.o: btreenode_linearSearchBigNode.cpp btreenode_linearSearchBigNode.hpp
	g++ -std=c++20 -Wall -Wextra -g -c btreenode_linearSearchBigNode.cpp -o $@

btree-optimized_linearSearchBig.o: btree_linearSearchBigNode.cpp btree_linearSearchBigNode.hpp 
	clang++ -std=c++20 -Wall -Wextra -g -c btree_linearSearchBigNode.cpp -o $@ -O3 -DNDEBUG

btreenode-optimized_linearSearchBig.o: btreenode_linearSearchBigNode.cpp btreenode_linearSearchBigNode.hpp
	clang++ -std=c++20 -Wall -Wextra -g -c btreenode_linearSearchBigNode.cpp -o $@ -O3 -DNDEBUG

utils_linearSearchBig.o: utils.hpp
	g++ -std=c++20 -Wall -Wextra -g -c utils.hpp -o $@

utils-optimized_linearSearchBig.o: utils.hpp
	clang++ -std=c++20 -Wall -Wextra -g -c utils.hpp -o $@

clean:
	rm -f btree_linearSearchBig.o btree_linearSearchBig.a btree-optimized_linearSearchBig.o btree-optimized_linearSearchBig.a btreenode_linearSearchBig.o btreenode-optimized_linearSearchBig.o utils_linearSearchBig.o utils-optimized_linearSearchBig.o