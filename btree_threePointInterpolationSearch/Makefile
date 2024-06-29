all: btree_threePointInterpolationSearch.a

btree_threePointInterpolationSearch.a: btree_threePointInterpolationSearch.o btreenode_threePointInterpolationSearch.o utils_threePointInterpolationSearch.o
	rm -f btree_threePointInterpolationSearch.a
	ar rcs btree_threePointInterpolationSearch.a btree_threePointInterpolationSearch.o btreenode_threePointInterpolationSearch.o utils_threePointInterpolationSearch.o

btree-optimized_threePointInterpolationSearch.a: btree-optimized_threePointInterpolationSearch.o btreenode-optimized_threePointInterpolationSearch.o utils-optimized_threePointInterpolationSearch.o
	rm -f btree-optimized_threePointInterpolationSearch.a
	ar rcs btree-optimized_threePointInterpolationSearch.a btree-optimized_threePointInterpolationSearch.o btreenode-optimized_threePointInterpolationSearch.o utils-optimized_threePointInterpolationSearch.o

btree_threePointInterpolationSearch.o: btree_threePointInterpolationSearch.cpp btree_threePointInterpolationSearch.hpp
	g++ -std=c++20 -Wall -Wextra -g -c btree_threePointInterpolationSearch.cpp -o $@

btreenode_threePointInterpolationSearch.o: btreenode_threePointInterpolationSearch.cpp btreenode_threePointInterpolationSearch.hpp
	g++ -std=c++20 -Wall -Wextra -g -c btreenode_threePointInterpolationSearch.cpp -o $@

btree-optimized_threePointInterpolationSearch.o: btree_threePointInterpolationSearch.cpp btree_threePointInterpolationSearch.hpp 
	g++ -std=c++20 -Wall -Wextra -g -c btree_threePointInterpolationSearch.cpp -o $@ -O3 -DNDEBUG

btreenode-optimized_threePointInterpolationSearch.o: btreenode_threePointInterpolationSearch.cpp btreenode_threePointInterpolationSearch.hpp
	g++ -std=c++20 -Wall -Wextra -g -c btreenode_threePointInterpolationSearch.cpp -o $@ -O3 -DNDEBUG

utils_threePointInterpolationSearch.o: utils.hpp
	g++ -std=c++20 -Wall -Wextra -g -c utils.hpp -o $@

utils-optimized_threePointInterpolationSearch.o: utils.hpp
	g++ -std=c++20 -Wall -Wextra -g -c utils.hpp -o $@

clean:
	rm -f btree_threePointInterpolationSearch.o btree_threePointInterpolationSearch.a btree-optimized_threePointInterpolationSearch.o btree-optimized_threePointInterpolationSearch.a btreenode_threePointInterpolationSearch.o btreenode-optimized_threePointInterpolationSearch.o utils_threePointInterpolationSearch.o utils-optimized_threePointInterpolationSearch.o