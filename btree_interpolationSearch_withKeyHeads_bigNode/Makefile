all: btree_interpolationSearchKeyHeadsBig.a

btree_interpolationSearchKeyHeadsBig.a: btree_interpolationSearchKeyHeadsBig.o btreenode_interpolationSearchKeyHeadsBig.o utils_interpolationSearchKeyHeadsBig.o
	rm -f btree_interpolationSearchKeyHeadsBig.a
	ar rcs btree_interpolationSearchKeyHeadsBig.a btree_interpolationSearchKeyHeadsBig.o btreenode_interpolationSearchKeyHeadsBig.o utils_interpolationSearchKeyHeadsBig.o

btree-optimized_interpolationSearchKeyHeadsBig.a: btree-optimized_interpolationSearchKeyHeadsBig.o btreenode-optimized_interpolationSearchKeyHeadsBig.o utils-optimized_interpolationSearchKeyHeadsBig.o
	rm -f btree-optimized_interpolationSearchKeyHeadsBig.a
	ar rcs btree-optimized_interpolationSearchKeyHeadsBig.a btree-optimized_interpolationSearchKeyHeadsBig.o btreenode-optimized_interpolationSearchKeyHeadsBig.o utils-optimized_interpolationSearchKeyHeadsBig.o

btree_interpolationSearchKeyHeadsBig.o: btree_interpolationSearchKeyHeadsBigNode.cpp btree_interpolationSearchKeyHeadsBigNode.hpp
	g++ -std=c++20 -Wall -Wextra -g -c btree_interpolationSearchKeyHeadsBigNode.cpp -o $@

btreenode_interpolationSearchKeyHeadsBig.o: btreenode_interpolationSearchKeyHeadsBigNode.cpp btreenode_interpolationSearchKeyHeadsBigNode.hpp
	g++ -std=c++20 -Wall -Wextra -g -c btreenode_interpolationSearchKeyHeadsBigNode.cpp -o $@

btree-optimized_interpolationSearchKeyHeadsBig.o: btree_interpolationSearchKeyHeadsBigNode.cpp btree_interpolationSearchKeyHeadsBigNode.hpp 
	clang++ -std=c++20 -Wall -Wextra -g -c btree_interpolationSearchKeyHeadsBigNode.cpp -o $@ -O3 -DNDEBUG

btreenode-optimized_interpolationSearchKeyHeadsBig.o: btreenode_interpolationSearchKeyHeadsBigNode.cpp btreenode_interpolationSearchKeyHeadsBigNode.hpp
	clang++ -std=c++20 -Wall -Wextra -g -c btreenode_interpolationSearchKeyHeadsBigNode.cpp -o $@ -O3 -DNDEBUG

utils_interpolationSearchKeyHeadsBig.o: utils.hpp
	g++ -std=c++20 -Wall -Wextra -g -c utils.hpp -o $@

utils-optimized_interpolationSearchKeyHeadsBig.o: utils.hpp
	clang++ -std=c++20 -Wall -Wextra -g -c utils.hpp -o $@

clean:
	rm -f btree_interpolationSearchKeyHeadsBig.o btree_interpolationSearchKeyHeadsBig.a btree-optimized_interpolationSearchKeyHeadsBig.o btree-optimized_interpolationSearchKeyHeadsBig.a btreenode_interpolationSearchKeyHeadsBig.o btreenode-optimized_interpolationSearchKeyHeadsBig.o utils_interpolationSearchKeyHeadsBig.o utils-optimized_interpolationSearchKeyHeadsBig.o