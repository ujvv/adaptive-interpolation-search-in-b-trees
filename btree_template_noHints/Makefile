all: btree-optimized_templatenoHints.a

btree_templatenoHints.a: btree_templatenoHints.o btreenode_templatenoHints.o utils_templatenoHints.o
	rm -f btree_templatenoHints.a
	ar rcs btree_templatenoHints.a btree_templatenoHints.o btreenode_templatenoHints.o utils_templatenoHints.o

btree-optimized_templatenoHints.a: btree-optimized_templatenoHints.o btreenode-optimized_templatenoHints.o utils_templatenoHints.o
	rm -f btree-optimized_templatenoHints.a
	ar rcs btree-optimized_templatenoHints.a btree-optimized_templatenoHints.o btreenode-optimized_templatenoHints.o utils_templatenoHints.o

btree_templatenoHints.o: btree_template.cpp btree_template.hpp
	clang++ -std=c++20 -Wall -Wextra -g -c btree_template.cpp -o $@

btreenode_templatenoHints.o: btreenode_template.cpp btreenode_template.hpp
	clang++ -std=c++20 -Wall -Wextra -g -c btreenode_template.cpp -o $@

btree-optimized_templatenoHints.o: btree_template.cpp btree_template.hpp 
	clang++ -std=c++20 -Wall -Wextra -g -c btree_template.cpp -o $@ -O3 -DNDEBUG

btreenode-optimized_templatenoHints.o: btreenode_template.cpp btreenode_template.hpp
	clang++ -std=c++20 -Wall -Wextra -g -c btreenode_template.cpp -o $@ -O3 -DNDEBUG

utils_templatenoHints.o: utils.hpp
	clang++ -std=c++20 -Wall -Wextra -g -c utils.hpp -o $@

clean:
	rm -f btree_templatenoHints.o btree_templatenoHints.a btree-optimized_templatenoHints.o btree-optimized_templatenoHints.a btreenode_templatenoHints.o btreenode-optimized_templatenoHints.o utils_templatenoHints.o