all: btree_interpolationSearchKeyHeads.a

btree_interpolationSearchKeyHeads.a: btree_interpolationSearchKeyHeads.o btreenode_interpolationSearchKeyHeads.o utils_interpolationSearchKeyHeads.o
	rm -f btree_interpolationSearchKeyHeads.a
	ar rcs btree_interpolationSearchKeyHeads.a btree_interpolationSearchKeyHeads.o btreenode_interpolationSearchKeyHeads.o utils_interpolationSearchKeyHeads.o

btree-optimized_interpolationSearchKeyHeads.a: btree-optimized_interpolationSearchKeyHeads.o btreenode-optimized_interpolationSearchKeyHeads.o utils-optimized_interpolationSearchKeyHeads.o
	rm -f btree-optimized_interpolationSearchKeyHeads.a
	ar rcs btree-optimized_interpolationSearchKeyHeads.a btree-optimized_interpolationSearchKeyHeads.o btreenode-optimized_interpolationSearchKeyHeads.o utils-optimized_interpolationSearchKeyHeads.o

btree_interpolationSearchKeyHeads.o: btree_interpolationSearchKeyHeads.cpp btree_interpolationSearchKeyHeads.hpp
	g++ -std=c++20 -Wall -Wextra -g -c btree_interpolationSearchKeyHeads.cpp -o $@

btreenode_interpolationSearchKeyHeads.o: btreenode_interpolationSearchKeyHeads.cpp btreenode_interpolationSearchKeyHeads.hpp
	g++ -std=c++20 -Wall -Wextra -g -c btreenode_interpolationSearchKeyHeads.cpp -o $@

btree-optimized_interpolationSearchKeyHeads.o: btree_interpolationSearchKeyHeads.cpp btree_interpolationSearchKeyHeads.hpp 
	g++ -std=c++20 -Wall -Wextra -g -c btree_interpolationSearchKeyHeads.cpp -o $@ -O3 -DNDEBUG

btreenode-optimized_interpolationSearchKeyHeads.o: btreenode_interpolationSearchKeyHeads.cpp btreenode_interpolationSearchKeyHeads.hpp
	g++ -std=c++20 -Wall -Wextra -g -c btreenode_interpolationSearchKeyHeads.cpp -o $@ -O3 -DNDEBUG

utils_interpolationSearchKeyHeads.o: utils.hpp
	g++ -std=c++20 -Wall -Wextra -g -c utils.hpp -o $@

utils-optimized_interpolationSearchKeyHeads.o: utils.hpp
	g++ -std=c++20 -Wall -Wextra -g -c utils.hpp -o $@

clean:
	rm -f btree_interpolationSearchKeyHeads.o btree_interpolationSearchKeyHeads.a btree-optimized_interpolationSearchKeyHeads.o btree-optimized_interpolationSearchKeyHeads.a btreenode_interpolationSearchKeyHeads.o btreenode-optimized_interpolationSearchKeyHeads.o utils_interpolationSearchKeyHeads.o utils-optimized_interpolationSearchKeyHeads.o