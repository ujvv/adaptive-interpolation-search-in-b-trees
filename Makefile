all:main

btree_template/btree_template.a: .FORCE
	cd btree_template;make btree_template.a

btree_template/btree-optimized_template.a: .FORCE
	cd btree_template;make btree-optimized_template.a

btree_template_bigNode/btree_templateBig.a: .FORCE
	cd btree_template_bigNode;make btree_templateBig.a

btree_template_bigNode/btree-optimized_templateBig.a: .FORCE
	cd btree_template_bigNode;make btree-optimized_templateBig.a

btree_binarySearch_noPrefix/btree_binarySearchNoPrefix.a: .FORCE
	cd btree_binarySearch_noPrefix;make btree_binarySearchNoPrefix.a

btree_binarySearch_noPrefix/btree-optimized_binarySearchNoPrefix.a: .FORCE
	cd btree_binarySearch_noPrefix;make btree-optimized_binarySearchNoPrefix.a

btree_binarySearch/btree_binarySearch.a: .FORCE
	cd btree_binarySearch;make btree_binarySearch.a

btree_binarySearch/btree-optimized_binarySearch.a: .FORCE
	cd btree_binarySearch;make btree-optimized_binarySearch.a

btree_binarySearch_bigNode/btree_binarySearchBig.a: .FORCE
	cd btree_binarySearch_bigNode;make btree_binarySearchBig.a

btree_binarySearch_bigNode/btree-optimized_binarySearchBig.a: .FORCE
	cd btree_binarySearch_bigNode;make btree-optimized_binarySearchBig.a

btree_binarySearch_withHints/btree_binarySearchHints.a: .FORCE
	cd btree_binarySearch_withHints;make btree_binarySearchHints.a

btree_binarySearch_withHints/btree-optimized_binarySearchHints.a: .FORCE
	cd btree_binarySearch_withHints;make btree-optimized_binarySearchHints.a

btree_linearSearch/btree_linearSearch.a: .FORCE
	cd btree_linearSearch;make btree_linearSearch.a

btree_linearSearch/btree-optimized_linearSearch.a: .FORCE
	cd btree_linearSearch;make btree-optimized_linearSearch.a

btree_linearSearch_bigNode/btree_linearSearchBig.a: .FORCE
	cd btree_linearSearch_bigNode;make btree_linearSearchBig.a

btree_linearSearch_bigNode/btree-optimized_linearSearchBig.a: .FORCE
	cd btree_linearSearch_bigNode;make btree-optimized_linearSearchBig.a

btree_interpolationSearch/btree_interpolationSearch.a: .FORCE
	cd btree_interpolationSearch;make btree_interpolationSearch.a

btree_interpolationSearch/btree-optimized_interpolationSearch.a: .FORCE
	cd btree_interpolationSearch;make btree-optimized_interpolationSearch.a

btree_interpolationSearch_bigNode/btree_interpolationSearchBig.a: .FORCE
	cd btree_interpolationSearch_bigNode;make btree_interpolationSearchBig.a

btree_interpolationSearch_bigNode/btree-optimized_interpolationSearchBig.a: .FORCE
	cd btree_interpolationSearch_bigNode;make btree-optimized_interpolationSearchBig.a

btree_interpolationSearch_withKeyHeads/btree_interpolationSearchKeyHeads.a: .FORCE
	cd btree_interpolationSearch_withKeyHeads;make btree_interpolationSearchKeyHeads.a

btree_interpolationSearch_withKeyHeads/btree-optimized_interpolationSearchKeyHeads.a: .FORCE
	cd btree_interpolationSearch_withKeyHeads;make btree-optimized_interpolationSearchKeyHeads.a

btree_interpolationSearch_withKeyHeads_bigNode/btree_interpolationSearchKeyHeadsBig.a: .FORCE
	cd btree_interpolationSearch_withKeyHeads_bigNode;make btree_interpolationSearchKeyHeadsBig.a

btree_interpolationSearch_withKeyHeads_bigNode/btree-optimized_interpolationSearchKeyHeadsBig.a: .FORCE
	cd btree_interpolationSearch_withKeyHeads_bigNode;make btree-optimized_interpolationSearchKeyHeadsBig.a

btree_plain_finished/btree_finished.a: .FORCE
	cd btree_plain_finished;make btree_finished.a

btree_plain_finished/btree-optimized_finished.a: .FORCE
	cd btree_plain_finished;make btree-optimized_finished.a

main: test_main.cpp test_main.hpp btree_template/btree_template.a btree_template_bigNode/btree_templateBig.a btree_plain_finished/btree_finished.a btree_binarySearch_noPrefix/btree_binarySearchNoPrefix.a btree_binarySearch/btree_binarySearch.a btree_binarySearch_bigNode/btree_binarySearchBig.a btree_binarySearch_withHints/btree_binarySearchHints.a btree_linearSearch/btree_linearSearch.a btree_linearSearch_bigNode/btree_linearSearchBig.a btree_interpolationSearch/btree_interpolationSearch.a btree_interpolationSearch_bigNode/btree_interpolationSearchBig.a btree_interpolationSearch_withKeyHeads/btree_interpolationSearchKeyHeads.a btree_interpolationSearch_withKeyHeads_bigNode/btree_interpolationSearchKeyHeadsBig.a tester.hpp PerfEvent.hpp
	g++ -o $@ -std=c++20 -Wall -Wextra -g $< btree_template/btree_template.a btree_template_bigNode/btree_templateBig.a btree_plain_finished/btree_finished.a btree_binarySearch_noPrefix/btree_binarySearchNoPrefix.a btree_binarySearch/btree_binarySearch.a btree_binarySearch_bigNode/btree_binarySearchBig.a btree_binarySearch_withHints/btree_binarySearchHints.a btree_linearSearch/btree_linearSearch.a btree_linearSearch_bigNode/btree_linearSearchBig.a btree_interpolationSearch/btree_interpolationSearch.a btree_interpolationSearch_bigNode/btree_interpolationSearchBig.a btree_interpolationSearch_withKeyHeads/btree_interpolationSearchKeyHeads.a btree_interpolationSearch_withKeyHeads_bigNode/btree_interpolationSearchKeyHeadsBig.a

main-optimized: test_main.cpp test_main.hpp btree_template/btree-optimized_template.a btree_template_bigNode/btree-optimized_templateBig.a btree_plain_finished/btree-optimized_finished.a btree_binarySearch_noPrefix/btree-optimized_binarySearchNoPrefix.a btree_binarySearch/btree-optimized_binarySearch.a btree_binarySearch_bigNode/btree-optimized_binarySearchBig.a btree_binarySearch_withHints/btree-optimized_binarySearchHints.a btree_linearSearch/btree-optimized_linearSearch.a btree_linearSearch_bigNode/btree-optimized_linearSearchBig.a btree_interpolationSearch/btree-optimized_interpolationSearch.a btree_interpolationSearch_bigNode/btree-optimized_interpolationSearchBig.a btree_interpolationSearch_withKeyHeads/btree-optimized_interpolationSearchKeyHeads.a btree_interpolationSearch_withKeyHeads_bigNode/btree-optimized_interpolationSearchKeyHeadsBig.a tester.hpp PerfEvent.hpp
	g++ -o $@ -std=c++20 -Wall -Wextra -g $< btree_template/btree-optimized_template.a btree_template_bigNode/btree-optimized_templateBig.a btree_plain_finished/btree-optimized_finished.a btree_binarySearch_noPrefix/btree-optimized_binarySearchNoPrefix.a btree_binarySearch/btree-optimized_binarySearch.a btree_binarySearch_bigNode/btree-optimized_binarySearchBig.a btree_binarySearch_withHints/btree-optimized_binarySearchHints.a btree_linearSearch/btree-optimized_linearSearch.a btree_linearSearch_bigNode/btree-optimized_linearSearchBig.a btree_interpolationSearch/btree-optimized_interpolationSearch.a btree_interpolationSearch_bigNode/btree-optimized_interpolationSearchBig.a btree_interpolationSearch_withKeyHeads/btree-optimized_interpolationSearchKeyHeads.a btree_interpolationSearch_withKeyHeads_bigNode/btree-optimized_interpolationSearchKeyHeadsBig.a


format:
	find . -type f -name '*.?pp' -exec clang-format -i {} \;

clean:
	rm -f main
	rm -f main-optimized
	cd btree_template; make clean
	cd btree_template_bigNode; make clean
	cd btree_plain_finished; make clean
	cd btree_binarySearch_noPrefix; make clean
	cd btree_binarySearch; make clean
	cd btree_binarySearch_bigNode; make clean
	cd btree_binarySearch_withHints; make clean
	cd btree_linearSearch; make clean
	cd btree_linearSearch_bigNode; make clean
	cd btree_interpolationSearch; make clean
	cd btree_interpolationSearch_bigNode; make clean
	cd btree_interpolationSearch_withKeyHeads; make clean
	cd btree_interpolationSearch_withKeyHeads_bigNode; make clean

.PHONY: .FORCE format
.FORCE:

