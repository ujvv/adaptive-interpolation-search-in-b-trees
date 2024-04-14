all: btree_templateBig.a

btree_templateBig.a: btree_templateBig.o btreenode_templateBig.o utils_templateBig.o
	rm -f btree_templateBig.a
	ar rcs btree_templateBig.a btree_templateBig.o btreenode_templateBig.o utils_templateBig.o

btree-optimized_templateBig.a: btree-optimized_templateBig.o btreenode-optimized_templateBig.o utils_templateBig.o
	rm -f btree-optimized_templateBig.a
	ar rcs btree-optimized_templateBig.a btree-optimized_templateBig.o btreenode-optimized_templateBig.o utils_templateBig.o

btree_templateBig.o: btree_template.cpp btree_template.hpp
	g++ -std=c++20 -Wall -Wextra -g -c btree_template.cpp -o $@

btreenode_templateBig.o: btreenode_template.cpp btreenode_template.hpp
	g++ -std=c++20 -Wall -Wextra -g -c btreenode_template.cpp -o $@

btree-optimized_templateBig.o: btree_template.cpp btree_template.hpp 
	clang++ -std=c++20 -Wall -Wextra -g -c btree_template.cpp -o $@ -O3 -DNDEBUG

btreenode-optimized_templateBig.o: btreenode_template.cpp btreenode_template.hpp
	clang++ -std=c++20 -Wall -Wextra -g -c btreenode_template.cpp -o $@ -O3 -DNDEBUG

utils_templateBig.o: utils.hpp
	g++ -std=c++20 -Wall -Wextra -g -c utils.hpp -o $@

clean:
	rm -f btree_templateBig.o btree_templateBig.a btree-optimized_templateBig.o btree-optimized_templateBig.a btreenode_templateBig.o btreenode-optimized_templateBig.o utils_templateBig.o