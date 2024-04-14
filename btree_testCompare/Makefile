all: btree_test.a

btree_test.a: btree_test.o btreenode_test.o utils_test.o
	rm -f btree_test.a
	ar rcs btree_test.a btree_test.o btreenode_test.o utils_test.o

btree-optimized_test.a: btree-optimized_test.o btreenode-optimized_test.o utils_test.o
	rm -f btree-optimized_test.a
	ar rcs btree-optimized_test.a btree-optimized_test.o btreenode-optimized_test.o utils_test.o

btree_test.o: btree.cpp btree.hpp
	g++ -std=c++20 -Wall -Wextra -g -c btree.cpp -o $@

btreenode_test.o: btreenode.cpp btreenode.hpp
	g++ -std=c++20 -Wall -Wextra -g -c btreenode.cpp -o $@

btree-optimized_test.o: btree.cpp btree.hpp 
	g++ -std=c++20 -Wall -Wextra -g -c btree.cpp -o $@ -O3 -DNDEBUG

btreenode-optimized_test.o: btreenode.cpp btreenode.hpp
	g++ -std=c++20 -Wall -Wextra -g -c btreenode.cpp -o $@ -O3 -DNDEBUG

utils_test.o: utils.hpp
	g++ -std=c++20 -Wall -Wextra -g -c utils.hpp -o $@

clean:
	rm -f btree_test.o btree_test.a btree-optimized_test.o btree-optimized_test.a btreenode_test.o btreenode-optimized_test.o utils_test.o