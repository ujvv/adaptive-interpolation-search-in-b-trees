all:main

btree_template/btree_template.a: .FORCE
	cd btree_template;make btree_template.a

btree_template/btree-optimized_template.a: .FORCE
	cd btree_template;make btree-optimized_template.a

#btree_template_noHints/btree_templatenoHints.a: .FORCE
#	cd btree_template_noHints;make btree_templatenoHints.a

#btree_template_noHints/btree-optimized_templatenoHints.a: .FORCE
#	cd btree_template_noHints;make btree-optimized_templatenoHints.a

#btree_template_bigNode/btree_templateBig.a: .FORCE
#	cd btree_template_bigNode;make btree_templateBig.a

#btree_template_bigNode/btree-optimized_templateBig.a: .FORCE
#	cd btree_template_bigNode;make btree-optimized_templateBig.a

btree_binarySearch/btree_binarySearch.a: .FORCE
	cd btree_binarySearch;make btree_binarySearch.a

btree_binarySearch/btree-optimized_binarySearch.a: .FORCE
	cd btree_binarySearch;make btree-optimized_binarySearch.a

btree_plain_finished/btree_finished.a: .FORCE
	cd btree_plain_finished;make btree_finished.a

btree_plain_finished/btree-optimized_finished.a: .FORCE
	cd btree_plain_finished;make btree-optimized_finished.a

main: test_main.cpp btree_template/btree_template.a btree_plain_finished/btree_finished.a btree_binarySearch/btree_binarySearch.a tester.hpp PerfEvent.hpp
	g++ -o $@ -std=c++20 -Wall -Wextra -g $< btree_template/btree_template.a btree_plain_finished/btree_finished.a btree_binarySearch/btree_binarySearch.a

main-optimized: test_main.cpp btree_template_noHints/btree-optimized_templatenoHints.a btree_innerNode_eytzinger_rmvLastBranch/btree-optimized_lastbranch.a tester.hpp PerfEvent.hpp
	clang++ -o $@ -std=c++20 -Wall -Wextra -g $< btree_template_noHints/btree-optimized_templatenoHints.a btree_innerNode_eytzinger_rmvLastBranch/btree-optimized_lastbranch.a


format:
	find . -type f -name '*.?pp' -exec clang-format -i {} \;

clean:
	rm -f main
	rm -f main-optimized
	cd btree_template; make clean
	cd btree_template_bigNode; make clean
	cd btree_testCompare; make clean
	cd btree_plain_finished; make clean
	cd btree_binarySearch; make clean

.PHONY: .FORCE format
.FORCE:

