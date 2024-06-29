all: btree_interpolationSequentialSearch.a

btree_interpolationSequentialSearch.a: btree_interpolationSequentialSearch.o btreenode_interpolationSequentialSearch.o utils_interpolationSequentialSearch.o
	rm -f btree_interpolationSequentialSearch.a
	ar rcs btree_interpolationSequentialSearch.a btree_interpolationSequentialSearch.o btreenode_interpolationSequentialSearch.o utils_interpolationSequentialSearch.o

btree-optimized_interpolationSequentialSearch.a: btree-optimized_interpolationSequentialSearch.o btreenode-optimized_interpolationSequentialSearch.o utils-optimized_interpolationSequentialSearch.o
	rm -f btree-optimized_interpolationSequentialSearch.a
	ar rcs btree-optimized_interpolationSequentialSearch.a btree-optimized_interpolationSequentialSearch.o btreenode-optimized_interpolationSequentialSearch.o utils-optimized_interpolationSequentialSearch.o

btree_interpolationSequentialSearch.o: btree_interpolationSequentialSearch.cpp btree_interpolationSequentialSearch.hpp
	g++ -std=c++20 -Wall -Wextra -g -c btree_interpolationSequentialSearch.cpp -o $@

btreenode_interpolationSequentialSearch.o: btreenode_interpolationSequentialSearch.cpp btreenode_interpolationSequentialSearch.hpp
	g++ -std=c++20 -Wall -Wextra -g -c btreenode_interpolationSequentialSearch.cpp -o $@

btree-optimized_interpolationSequentialSearch.o: btree_interpolationSequentialSearch.cpp btree_interpolationSequentialSearch.hpp 
	g++ -std=c++20 -Wall -Wextra -g -c btree_interpolationSequentialSearch.cpp -o $@ -O3 -DNDEBUG

btreenode-optimized_interpolationSequentialSearch.o: btreenode_interpolationSequentialSearch.cpp btreenode_interpolationSequentialSearch.hpp
	g++ -std=c++20 -Wall -Wextra -g -c btreenode_interpolationSequentialSearch.cpp -o $@ -O3 -DNDEBUG

utils_interpolationSequentialSearch.o: utils.hpp
	g++ -std=c++20 -Wall -Wextra -g -c utils.hpp -o $@

utils-optimized_interpolationSequentialSearch.o: utils.hpp
	g++ -std=c++20 -Wall -Wextra -g -c utils.hpp -o $@

clean:
	rm -f btree_interpolationSequentialSearch.o btree_interpolationSequentialSearch.a btree-optimized_interpolationSequentialSearch.o btree-optimized_interpolationSequentialSearch.a btreenode_interpolationSequentialSearch.o btreenode-optimized_interpolationSequentialSearch.o utils_interpolationSequentialSearch.o utils-optimized_interpolationSequentialSearch.o