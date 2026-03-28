#include <iostream>
#include <catch2/catch_test_macros.hpp>
#include <vector>

#include "../src/HexGrid.hpp"
#include "../src/Algorithms/Algorithm.hpp"
#include "../src/Algorithms/Dijkstras.hpp"
#include "../src/Algorithms/AStar.hpp"

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
	REQUIRE(test.getStart() == std::pair<int, int>{0,9});
	REQUIRE(test.getEnd() == std::pair<int, int>{9,0});
}

TEST_CASE("Init All", "[hexgrid]") {
	HexGrid test({10, 10}, {1, 1}, {2, 1});
	REQUIRE(test.getStart() == std::pair<int, int>{1,1});
	REQUIRE(test.getEnd() == std::pair<int, int>{2,1});
}

TEST_CASE("Top Left", "[neighbors]") {
	HexGrid test({10, 10});
	std::vector<std::pair<int, int>> neighbors = test.getNeighbors({0, 0});
	// Should yield {right, bottom right}
	REQUIRE(neighbors[0] == std::pair<int, int>{1, 0});
	REQUIRE(neighbors[1] == std::pair<int, int>{0, 1});
	REQUIRE(neighbors.size() == 2);
}

TEST_CASE("Top Right", "[neighbors]") {
	HexGrid test({10, 10});
	std::vector<std::pair<int, int>> neighbors = test.getNeighbors({9, 0});
	// Should yield {left, bottom left, bottom right}
	REQUIRE(neighbors[0] == std::pair<int, int>{8, 0});
	REQUIRE(neighbors[1] == std::pair<int, int>{8, 1});
	REQUIRE(neighbors[2] == std::pair<int, int>{9, 1});
	REQUIRE(neighbors.size() == 3);
}

TEST_CASE("Bottom Left, Odd Height", "[neighbors]") {
	HexGrid test({4, 4});
	std::vector<std::pair<int, int>> neighbors = test.getNeighbors({0, 3});
	// Should yield {top left, top right, right}
	REQUIRE(neighbors[0] == std::pair<int, int>{0, 2});
	REQUIRE(neighbors[1] == std::pair<int, int>{1, 2});
	REQUIRE(neighbors[2] == std::pair<int, int>{1, 3});
	REQUIRE(neighbors.size() == 3);
}

TEST_CASE("Bottom Right, Odd Height", "[neighbors]") {
	HexGrid test({4, 4});
	std::vector<std::pair<int, int>> neighbors = test.getNeighbors({3, 3});
	// Should yield {top left, left}
	REQUIRE(neighbors[0] == std::pair<int, int>{3, 2});
	REQUIRE(neighbors[1] == std::pair<int, int>{2, 3});
	REQUIRE(neighbors.size() == 2);
}

TEST_CASE("Bottom Left, Even Height", "[neighbors]") {
	HexGrid test({5, 5});
	std::vector<std::pair<int, int>> neighbors = test.getNeighbors({0, 4});
	// Should yield {top right, right}
	REQUIRE(neighbors[0] == std::pair<int, int>{0, 3});
	REQUIRE(neighbors[1] == std::pair<int, int>{1, 4});
	REQUIRE(neighbors.size() == 2);
}

TEST_CASE("Bottom Right, Even Height", "[neighbors]") {
	HexGrid test({5, 5});
	std::vector<std::pair<int, int>> neighbors = test.getNeighbors({4, 4});
	// Should yield {top left, top right, left}
	REQUIRE(neighbors[0] == std::pair<int, int>{3, 3});
	REQUIRE(neighbors[1] == std::pair<int, int>{4, 3});
	REQUIRE(neighbors[2] == std::pair<int, int>{3, 4});
	REQUIRE(neighbors.size() == 3);
}

TEST_CASE("Middle, Odd Coord", "[neighbors]") {
	HexGrid test({100, 100});
	std::vector<std::pair<int, int>> neighbors = test.getNeighbors({5, 5});
	std::vector<std::pair<int, int>> coords = {{5,4}, {6,4}, {4,5}, {6,5}, {5,6}, {6,6}};
	for (int i = 0; i < coords.size(); i++) {
		REQUIRE(coords[i] == neighbors[i]);
	}
	REQUIRE(neighbors.size() == 6);
}

TEST_CASE("Middle, Even Coord", "[neighbors]") {
	HexGrid test({10, 10});
	std::vector<std::pair<int, int>> neighbors = test.getNeighbors({4, 4});
	std::vector<std::pair<int, int>> coords = {{3,3}, {4,3}, {3,4}, {5,4}, {3,5}, {4,5}};
	for (int i = 0; i < coords.size(); i++) {
		REQUIRE(coords[i] == neighbors[i]);
	}
	REQUIRE(neighbors.size() == 6);
}

TEST_CASE("Finds path", "[Algorithms]")
{
	Vec2d size{10, 10};
	HexGrid testGrid(size);
	std::vector<std::unique_ptr<Algorithm>> algorithms;
	algorithms.emplace_back(std::make_unique<Dijkstras>(Vec2d{0, 0}, Vec2d{size.first - 1, size.second -1}, &testGrid));
	algorithms.emplace_back(std::make_unique<AStar>(Vec2d{0, 0}, Vec2d{size.first - 1, size.second -1}, &testGrid));

	std::vector<std::vector<Vec2d>> foundPaths;
	for (std::unique_ptr<Algorithm>& algo : algorithms)
	{
		foundPaths.emplace_back(algo->runAlgorithm());

	}
}