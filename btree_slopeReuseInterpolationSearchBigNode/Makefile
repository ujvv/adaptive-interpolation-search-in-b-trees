all: btree_slopeReuseInterpolationSearchBigNode.a

btree_slopeReuseInterpolationSearchBigNode.a: btree_slopeReuseInterpolationSearchBigNode.o btreenode_slopeReuseInterpolationSearchBigNode.o utils_slopeReuseInterpolationSearchBigNode.o
	rm -f btree_slopeReuseInterpolationSearchBigNode.a
	ar rcs btree_slopeReuseInterpolationSearchBigNode.a btree_slopeReuseInterpolationSearchBigNode.o btreenode_slopeReuseInterpolationSearchBigNode.o utils_slopeReuseInterpolationSearchBigNode.o

btree-optimized_slopeReuseInterpolationSearchBigNode.a: btree-optimized_slopeReuseInterpolationSearchBigNode.o btreenode-optimized_slopeReuseInterpolationSearchBigNode.o utils-optimized_slopeReuseInterpolationSearchBigNode.o
	rm -f btree-optimized_slopeReuseInterpolationSearchBigNode.a
	ar rcs btree-optimized_slopeReuseInterpolationSearchBigNode.a btree-optimized_slopeReuseInterpolationSearchBigNode.o btreenode-optimized_slopeReuseInterpolationSearchBigNode.o utils-optimized_slopeReuseInterpolationSearchBigNode.o

btree_slopeReuseInterpolationSearchBigNode.o: btree_slopeReuseInterpolationSearchBigNode.cpp btree_slopeReuseInterpolationSearchBigNode.hpp
	g++ -std=c++20 -Wall -Wextra -g -c btree_slopeReuseInterpolationSearchBigNode.cpp -o $@

btreenode_slopeReuseInterpolationSearchBigNode.o: btreenode_slopeReuseInterpolationSearchBigNode.cpp btreenode_slopeReuseInterpolationSearchBigNode.hpp
	g++ -std=c++20 -Wall -Wextra -g -c btreenode_slopeReuseInterpolationSearchBigNode.cpp -o $@

btree-optimized_slopeReuseInterpolationSearchBigNode.o: btree_slopeReuseInterpolationSearchBigNode.cpp btree_slopeReuseInterpolationSearchBigNode.hpp 
	g++ -std=c++20 -Wall -Wextra -g -c btree_slopeReuseInterpolationSearchBigNode.cpp -o $@ -O3 -DNDEBUG

btreenode-optimized_slopeReuseInterpolationSearchBigNode.o: btreenode_slopeReuseInterpolationSearchBigNode.cpp btreenode_slopeReuseInterpolationSearchBigNode.hpp
	g++ -std=c++20 -Wall -Wextra -g -c btreenode_slopeReuseInterpolationSearchBigNode.cpp -o $@ -O3 -DNDEBUG

utils_slopeReuseInterpolationSearchBigNode.o: utils.hpp
	g++ -std=c++20 -Wall -Wextra -g -c utils.hpp -o $@

utils-optimized_slopeReuseInterpolationSearchBigNode.o: utils.hpp
	g++ -std=c++20 -Wall -Wextra -g -c utils.hpp -o $@

clean:
	rm -f btree_slopeReuseInterpolationSearchBigNode.o btree_slopeReuseInterpolationSearchBigNode.a btree-optimized_slopeReuseInterpolationSearchBigNode.o btree-optimized_slopeReuseInterpolationSearchBigNode.a btreenode_slopeReuseInterpolationSearchBigNode.o btreenode-optimized_slopeReuseInterpolationSearchBigNode.o utils_slopeReuseInterpolationSearchBigNode.o utils-optimized_slopeReuseInterpolationSearchBigNode.o