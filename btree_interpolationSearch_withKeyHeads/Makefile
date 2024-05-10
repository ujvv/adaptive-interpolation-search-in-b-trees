all: btree_interpolationSearch.a

btree_interpolationSearch.a: btree_interpolationSearch.o btreenode_interpolationSearch.o utils_interpolationSearch.o
	rm -f btree_interpolationSearch.a
	ar rcs btree_interpolationSearch.a btree_interpolationSearch.o btreenode_interpolationSearch.o utils_interpolationSearch.o

btree-optimized_interpolationSearch.a: btree-optimized_interpolationSearch.o btreenode-optimized_interpolationSearch.o utils_interpolationSearch.o
	rm -f btree-optimized_interpolationSearch.a
	ar rcs btree-optimized_interpolationSearch.a btree-optimized_interpolationSearch.o btreenode-optimized_interpolationSearch.o utils_interpolationSearch.o

btree_interpolationSearch.o: btree_interpolationSearch.cpp btree_interpolationSearch.hpp
	g++ -std=c++20 -Wall -Wextra -g -c btree_interpolationSearch.cpp -o $@

btreenode_interpolationSearch.o: btreenode_interpolationSearch.cpp btreenode_interpolationSearch.hpp
	g++ -std=c++20 -Wall -Wextra -g -c btreenode_interpolationSearch.cpp -o $@

btree-optimized_interpolationSearch.o: btree_interpolationSearch.cpp btree_interpolationSearch.hpp 
	clang++ -std=c++20 -Wall -Wextra -g -c btree_interpolationSearch.cpp -o $@ -O3 -DNDEBUG

btreenode-optimized_interpolationSearch.o: btreenode_interpolationSearch.cpp btreenode_interpolationSearch.hpp
	clang++ -std=c++20 -Wall -Wextra -g -c btreenode_interpolationSearch.cpp -o $@ -O3 -DNDEBUG

utils_interpolationSearch.o: utils.hpp
	g++ -std=c++20 -Wall -Wextra -g -c utils.hpp -o $@

clean:
	rm -f btree_interpolationSearch.o btree_interpolationSearch.a btree-optimized_interpolationSearch.o btree-optimized_interpolationSearch.a btreenode_interpolationSearch.o btreenode-optimized_interpolationSearch.o utils_interpolationSearch.o