all: btree_slopeReuseInterpolationSearch.a

btree_slopeReuseInterpolationSearch.a: btree_slopeReuseInterpolationSearch.o btreenode_slopeReuseInterpolationSearch.o utils_slopeReuseInterpolationSearch.o
	rm -f btree_slopeReuseInterpolationSearch.a
	ar rcs btree_slopeReuseInterpolationSearch.a btree_slopeReuseInterpolationSearch.o btreenode_slopeReuseInterpolationSearch.o utils_slopeReuseInterpolationSearch.o

btree-optimized_slopeReuseInterpolationSearch.a: btree-optimized_slopeReuseInterpolationSearch.o btreenode-optimized_slopeReuseInterpolationSearch.o utils-optimized_slopeReuseInterpolationSearch.o
	rm -f btree-optimized_slopeReuseInterpolationSearch.a
	ar rcs btree-optimized_slopeReuseInterpolationSearch.a btree-optimized_slopeReuseInterpolationSearch.o btreenode-optimized_slopeReuseInterpolationSearch.o utils-optimized_slopeReuseInterpolationSearch.o

btree_slopeReuseInterpolationSearch.o: btree_slopeReuseInterpolationSearch.cpp btree_slopeReuseInterpolationSearch.hpp
	g++ -std=c++20 -Wall -Wextra -g -c btree_slopeReuseInterpolationSearch.cpp -o $@

btreenode_slopeReuseInterpolationSearch.o: btreenode_slopeReuseInterpolationSearch.cpp btreenode_slopeReuseInterpolationSearch.hpp
	g++ -std=c++20 -Wall -Wextra -g -c btreenode_slopeReuseInterpolationSearch.cpp -o $@

btree-optimized_slopeReuseInterpolationSearch.o: btree_slopeReuseInterpolationSearch.cpp btree_slopeReuseInterpolationSearch.hpp 
	g++ -std=c++20 -Wall -Wextra -g -c btree_slopeReuseInterpolationSearch.cpp -o $@ -O3 -DNDEBUG

btreenode-optimized_slopeReuseInterpolationSearch.o: btreenode_slopeReuseInterpolationSearch.cpp btreenode_slopeReuseInterpolationSearch.hpp
	g++ -std=c++20 -Wall -Wextra -g -c btreenode_slopeReuseInterpolationSearch.cpp -o $@ -O3 -DNDEBUG

utils_slopeReuseInterpolationSearch.o: utils.hpp
	g++ -std=c++20 -Wall -Wextra -g -c utils.hpp -o $@

utils-optimized_slopeReuseInterpolationSearch.o: utils.hpp
	g++ -std=c++20 -Wall -Wextra -g -c utils.hpp -o $@

clean:
	rm -f btree_slopeReuseInterpolationSearch.o btree_slopeReuseInterpolationSearch.a btree-optimized_slopeReuseInterpolationSearch.o btree-optimized_slopeReuseInterpolationSearch.a btreenode_slopeReuseInterpolationSearch.o btreenode-optimized_slopeReuseInterpolationSearch.o utils_slopeReuseInterpolationSearch.o utils-optimized_slopeReuseInterpolationSearch.o