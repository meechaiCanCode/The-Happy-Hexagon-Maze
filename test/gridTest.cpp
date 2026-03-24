#include <catch2/catch_test_macros.hpp>
#include "../src/HexGrid.hpp"

TEST_CASE("Board Initializing", "[hexgrid]") {
	HexGrid test({10, 10});
	int num1s = 0;
	int num0s = 0;
	for (int i = 0; i < 10; i++) {
		for (int j = 0; j < 10; j++) {
			if ((test.grid)[i][j] == 1) num1s++;
			else num0s++;
		}
	}
	REQUIRE(num0s == 35);
	REQUIRE(num1s == 65);
	REQUIRE(test.getStart() == std::pair<int, int>{0,0});
/*
	for (Test& test : tests) {
		DYNAMIC_SECTION("Test: " << test.name) {
			vector<int> sortedUfids;
			for (const Student& s : test.insertionOrder) {
				sortedUfids.push_back(s.ufid);
			}
			std::sort(sortedUfids.begin(), sortedUfids.end());
			REQUIRE(sortedUfids == nodeVecToIntVec(test.bst.TraverseInorder()));
		}
	}*/
}