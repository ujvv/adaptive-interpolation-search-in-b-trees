all: btree_adaptiveSearch.a

btree_adaptiveSearch.a: btree_adaptiveSearch.o btreenode_adaptiveSearch.o utils_adaptiveSearch.o
	rm -f btree_adaptiveSearch.a
	ar rcs btree_adaptiveSearch.a btree_adaptiveSearch.o btreenode_adaptiveSearch.o utils_adaptiveSearch.o

btree-optimized_adaptiveSearch.a: btree-optimized_adaptiveSearch.o btreenode-optimized_adaptiveSearch.o utils-optimized_adaptiveSearch.o
	rm -f btree-optimized_adaptiveSearch.a
	ar rcs btree-optimized_adaptiveSearch.a btree-optimized_adaptiveSearch.o btreenode-optimized_adaptiveSearch.o utils-optimized_adaptiveSearch.o

btree_adaptiveSearch.o: btree_adaptiveSearch.cpp btree_adaptiveSearch.hpp
	g++ -std=c++20 -Wall -Wextra -g -c btree_adaptiveSearch.cpp -o $@

btreenode_adaptiveSearch.o: btreenode_adaptiveSearch.cpp btreenode_adaptiveSearch.hpp
	g++ -std=c++20 -Wall -Wextra -g -c btreenode_adaptiveSearch.cpp -o $@

btree-optimized_adaptiveSearch.o: btree_adaptiveSearch.cpp btree_adaptiveSearch.hpp 
	g++ -std=c++20 -Wall -Wextra -g -c btree_adaptiveSearch.cpp -o $@ -O3 -DNDEBUG

btreenode-optimized_adaptiveSearch.o: btreenode_adaptiveSearch.cpp btreenode_adaptiveSearch.hpp
	g++ -std=c++20 -Wall -Wextra -g -c btreenode_adaptiveSearch.cpp -o $@ -O3 -DNDEBUG

utils_adaptiveSearch.o: utils.hpp
	g++ -std=c++20 -Wall -Wextra -g -c utils.hpp -o $@

utils-optimized_adaptiveSearch.o: utils.hpp
	g++ -std=c++20 -Wall -Wextra -g -c utils.hpp -o $@

clean:
	rm -f btree_adaptiveSearch.o btree_adaptiveSearch.a btree-optimized_adaptiveSearch.o btree-optimized_adaptiveSearch.a btreenode_adaptiveSearch.o btreenode-optimized_adaptiveSearch.o utils_adaptiveSearch.o utils-optimized_adaptiveSearch.o