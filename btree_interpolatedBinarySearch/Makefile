all: btree_interpolatedBinarySearch.a

btree_interpolatedBinarySearch.a: btree_interpolatedBinarySearch.o btreenode_interpolatedBinarySearch.o utils_interpolatedBinarySearch.o
	rm -f btree_interpolatedBinarySearch.a
	ar rcs btree_interpolatedBinarySearch.a btree_interpolatedBinarySearch.o btreenode_interpolatedBinarySearch.o utils_interpolatedBinarySearch.o

btree-optimized_interpolatedBinarySearch.a: btree-optimized_interpolatedBinarySearch.o btreenode-optimized_interpolatedBinarySearch.o utils-optimized_interpolatedBinarySearch.o
	rm -f btree-optimized_interpolatedBinarySearch.a
	ar rcs btree-optimized_interpolatedBinarySearch.a btree-optimized_interpolatedBinarySearch.o btreenode-optimized_interpolatedBinarySearch.o utils-optimized_interpolatedBinarySearch.o

btree_interpolatedBinarySearch.o: btree_interpolatedBinarySearch.cpp btree_interpolatedBinarySearch.hpp
	g++ -std=c++20 -Wall -Wextra -g -c btree_interpolatedBinarySearch.cpp -o $@

btreenode_interpolatedBinarySearch.o: btreenode_interpolatedBinarySearch.cpp btreenode_interpolatedBinarySearch.hpp
	g++ -std=c++20 -Wall -Wextra -g -c btreenode_interpolatedBinarySearch.cpp -o $@

btree-optimized_interpolatedBinarySearch.o: btree_interpolatedBinarySearch.cpp btree_interpolatedBinarySearch.hpp 
	g++ -std=c++20 -Wall -Wextra -g -c btree_interpolatedBinarySearch.cpp -o $@ -O3 -DNDEBUG

btreenode-optimized_interpolatedBinarySearch.o: btreenode_interpolatedBinarySearch.cpp btreenode_interpolatedBinarySearch.hpp
	g++ -std=c++20 -Wall -Wextra -g -c btreenode_interpolatedBinarySearch.cpp -o $@ -O3 -DNDEBUG

utils_interpolatedBinarySearch.o: utils.hpp
	g++ -std=c++20 -Wall -Wextra -g -c utils.hpp -o $@

utils-optimized_interpolatedBinarySearch.o: utils.hpp
	g++ -std=c++20 -Wall -Wextra -g -c utils.hpp -o $@

clean:
	rm -f btree_interpolatedBinarySearch.o btree_interpolatedBinarySearch.a btree-optimized_interpolatedBinarySearch.o btree-optimized_interpolatedBinarySearch.a btreenode_interpolatedBinarySearch.o btreenode-optimized_interpolatedBinarySearch.o utils_interpolatedBinarySearch.o utils-optimized_interpolatedBinarySearch.o