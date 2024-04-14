all: btree_template.a

btree_template.a: btree_template.o btreenode_template.o utils_template.o
	rm -f btree_template.a
	ar rcs btree_template.a btree_template.o btreenode_template.o utils_template.o

btree-optimized_template.a: btree-optimized_template.o btreenode-optimized_template.o utils_template.o
	rm -f btree-optimized_template.a
	ar rcs btree-optimized_template.a btree-optimized_template.o btreenode-optimized_template.o utils_template.o

btree_template.o: btree_template.cpp btree_template.hpp
	g++ -std=c++20 -Wall -Wextra -g -c btree_template.cpp -o $@

btreenode_template.o: btreenode_template.cpp btreenode_template.hpp
	g++ -std=c++20 -Wall -Wextra -g -c btreenode_template.cpp -o $@

btree-optimized_template.o: btree_template.cpp btree_template.hpp 
	clang++ -std=c++20 -Wall -Wextra -g -c btree_template.cpp -o $@ -O3 -DNDEBUG

btreenode-optimized_template.o: btreenode_template.cpp btreenode_template.hpp
	clang++ -std=c++20 -Wall -Wextra -g -c btreenode_template.cpp -o $@ -O3 -DNDEBUG

utils_template.o: utils.hpp
	g++ -std=c++20 -Wall -Wextra -g -c utils.hpp -o $@

clean:
	rm -f btree_template.o btree_template.a btree-optimized_template.o btree-optimized_template.a btreenode_template.o btreenode-optimized_template.o utils_template.o