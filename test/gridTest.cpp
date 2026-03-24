#include <catch2/catch_test_macros.hpp>
#include "../src/HexGrid.hpp"

TEST_CASE("Init Just Size", "[hexgrid]") {
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
}

TEST_CASE("Init Size & %", "[hexgrid]") {
	HexGrid test({10, 10}, 0.25);
	int num1s = 0;
	int num0s = 0;
	for (int i = 0; i < 10; i++) {
		for (int j = 0; j < 10; j++) {
			if ((test.grid)[i][j] == 1) num1s++;
			else num0s++;
		}
	}
	REQUIRE(num0s == 25);
	REQUIRE(num1s == 75);
}

TEST_CASE("Init All", "[hexgrid]") {
	HexGrid test({10, 10}, 0.25, {1, 1}, {2, 1});
	int num1s = 0;
	int num0s = 0;
	for (int i = 0; i < 10; i++) {
		for (int j = 0; j < 10; j++) {
			if ((test.grid)[i][j] == 1) num1s++;
			else num0s++;
		}
	}
	REQUIRE(test.getStart() == std::pair<int, int>{1,1});
	REQUIRE(test.getEnd() == std::pair<int, int>{2,1});
}