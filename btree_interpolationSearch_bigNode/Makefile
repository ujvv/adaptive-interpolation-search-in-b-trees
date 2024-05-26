all: btree_interpolationSearchBig.a

btree_interpolationSearchBig.a: btree_interpolationSearchBig.o btreenode_interpolationSearchBig.o utils_interpolationSearchBig.o
	rm -f btree_interpolationSearchBig.a
	ar rcs btree_interpolationSearchBig.a btree_interpolationSearchBig.o btreenode_interpolationSearchBig.o utils_interpolationSearchBig.o

btree-optimized_interpolationSearchBig.a: btree-optimized_interpolationSearchBig.o btreenode-optimized_interpolationSearchBig.o utils-optimized_interpolationSearchBig.o
	rm -f btree-optimized_interpolationSearchBig.a
	ar rcs btree-optimized_interpolationSearchBig.a btree-optimized_interpolationSearchBig.o btreenode-optimized_interpolationSearchBig.o utils-optimized_interpolationSearchBig.o

btree_interpolationSearchBig.o: btree_interpolationSearchBigNode.cpp btree_interpolationSearchBigNode.hpp
	g++ -std=c++20 -Wall -Wextra -g -c btree_interpolationSearchBigNode.cpp -o $@

btreenode_interpolationSearchBig.o: btreenode_interpolationSearchBigNode.cpp btreenode_interpolationSearchBigNode.hpp
	g++ -std=c++20 -Wall -Wextra -g -c btreenode_interpolationSearchBigNode.cpp -o $@

btree-optimized_interpolationSearchBig.o: btree_interpolationSearchBigNode.cpp btree_interpolationSearchBigNode.hpp 
	clang++ -std=c++20 -Wall -Wextra -g -c btree_interpolationSearchBigNode.cpp -o $@ -O3 -DNDEBUG

btreenode-optimized_interpolationSearchBig.o: btreenode_interpolationSearchBigNode.cpp btreenode_interpolationSearchBigNode.hpp
	clang++ -std=c++20 -Wall -Wextra -g -c btreenode_interpolationSearchBigNode.cpp -o $@ -O3 -DNDEBUG

utils_interpolationSearchBig.o: utils.hpp
	g++ -std=c++20 -Wall -Wextra -g -c utils.hpp -o $@

utils-optimized_interpolationSearchBig.o: utils.hpp
	clang++ -std=c++20 -Wall -Wextra -g -c utils.hpp -o $@

clean:
	rm -f btree_interpolationSearchBig.o btree_interpolationSearchBig.a btree-optimized_interpolationSearchBig.o btree-optimized_interpolationSearchBig.a btreenode_interpolationSearchBig.o btreenode-optimized_interpolationSearchBig.o utils_interpolationSearchBig.o utils-optimized_interpolationSearchBig.o