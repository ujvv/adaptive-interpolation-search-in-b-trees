all: btree_templateBig.a

btree_templateBig.a: btree_templateBig.o btreenode_templateBig.o utils_templateBig.o
	rm -f btree_templateBig.a
	ar rcs btree_templateBig.a btree_templateBig.o btreenode_templateBig.o utils_templateBig.o

btree-optimized_templateBig.a: btree-optimized_templateBig.o btreenode-optimized_templateBig.o utils-optimized_templateBig.o
	rm -f btree-optimized_templateBig.a
	ar rcs btree-optimized_templateBig.a btree-optimized_templateBig.o btreenode-optimized_templateBig.o utils-optimized_templateBig.o

btree_templateBig.o: btree_templateBigNode.cpp btree_templateBigNode.hpp
	g++ -std=c++20 -Wall -Wextra -g -c btree_templateBigNode.cpp -o $@

btreenode_templateBig.o: btreenode_templateBigNode.cpp btreenode_templateBigNode.hpp
	g++ -std=c++20 -Wall -Wextra -g -c btreenode_templateBigNode.cpp -o $@

btree-optimized_templateBig.o: btree_templateBigNode.cpp btree_templateBigNode.hpp 
	g++ -std=c++20 -Wall -Wextra -g -c btree_templateBigNode.cpp -o $@ -O3 -DNDEBUG

btreenode-optimized_templateBig.o: btreenode_templateBigNode.cpp btreenode_templateBigNode.hpp
	g++ -std=c++20 -Wall -Wextra -g -c btreenode_templateBigNode.cpp -o $@ -O3 -DNDEBUG

utils_templateBig.o: utils.hpp
	g++ -std=c++20 -Wall -Wextra -g -c utils.hpp -o $@

utils-optimized_templateBig.o: utils.hpp
	g++ -std=c++20 -Wall -Wextra -g -c utils.hpp -o $@

clean:
	rm -f btree_templateBig.o btree_templateBig.a btree-optimized_templateBig.o btree-optimized_templateBig.a btreenode_templateBig.o btreenode-optimized_templateBig.o utils_templateBig.o utils-optimized_templateBig.o