all: btree_linearSearch.a

btree_linearSearch.a: btree_linearSearch.o btreenode_linearSearch.o utils_linearSearch.o
	rm -f btree_linearSearch.a
	ar rcs btree_linearSearch.a btree_linearSearch.o btreenode_linearSearch.o utils_linearSearch.o

btree-optimized_linearSearch.a: btree-optimized_linearSearch.o btreenode-optimized_linearSearch.o utils_linearSearch.o
	rm -f btree-optimized_linearSearch.a
	ar rcs btree-optimized_linearSearch.a btree-optimized_linearSearch.o btreenode-optimized_linearSearch.o utils_linearSearch.o

btree_linearSearch.o: btree_linearSearch.cpp btree_linearSearch.hpp
	g++ -std=c++20 -Wall -Wextra -g -c btree_linearSearch.cpp -o $@

btreenode_linearSearch.o: btreenode_linearSearch.cpp btreenode_linearSearch.hpp
	g++ -std=c++20 -Wall -Wextra -g -c btreenode_linearSearch.cpp -o $@

btree-optimized_linearSearch.o: btree_linearSearch.cpp btree_linearSearch.hpp 
	clang++ -std=c++20 -Wall -Wextra -g -c btree_linearSearch.cpp -o $@ -O3 -DNDEBUG

btreenode-optimized_linearSearch.o: btreenode_linearSearch.cpp btreenode_linearSearch.hpp
	clang++ -std=c++20 -Wall -Wextra -g -c btreenode_linearSearch.cpp -o $@ -O3 -DNDEBUG

utils_linearSearch.o: utils.hpp
	g++ -std=c++20 -Wall -Wextra -g -c utils.hpp -o $@

clean:
	rm -f btree_linearSearch.o btree_linearSearch.a btree-optimized_linearSearch.o btree-optimized_linearSearch.a btreenode_linearSearch.o btreenode-optimized_linearSearch.o utils_linearSearch.o