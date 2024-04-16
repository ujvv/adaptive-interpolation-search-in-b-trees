all: btree_finished.a

btree_finished.a: btree_finished.o
	rm -f btree_finished.a
	ar rcs btree_finished.a btree_finished.o

btree-optimized_finished.a: btree-optimized_finished.o
	rm -f btree-optimized_finished.a
	ar rcs btree-optimized_finished.a btree-optimized_finished.o

btree_finished.o:btree.cpp btree.hpp
	g++ -Wall -Wextra -g -std=c++20 -c btree.cpp -o $@

btree-optimized_finished.o: btree.cpp btree.hpp
	clang++ -Wall -Wextra -g -std=c++20 -c btree.cpp -o $@ -O3 -DNDEBUG

clean:
	rm -f btree_finished.o btree_finished.a btree-optimized_finished.o btree-optimized_finished.a
