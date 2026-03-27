#include <iostream>
#include <unordered_set>
#include "HexGrid.hpp"
#include <ctime>
#include <memory>

#include "Algorithms/Algorithm.hpp"
#include "Algorithms/AStar.hpp"
#include "Algorithms/Dijkstras.hpp"

int main() {
    /* I think the order of the main should be as follows:
     1. Boot GUI
     2. Allow user to pick grid size, start, finish, dead cell percentage, and algorithms
     3. Send info to gridTest.hpp for init
     4.
     */
    std::pair<int, int> size = {6,4};
    const std::clock_t start_time = clock();
    HexGrid bigOh{size, 0.5};
    const std::clock_t end_time = clock();
    for (int i = 0; i < size.second; i++) {
        std::cout << "\n";
        if (i % 2 == 1) std::cout << " ";
        for (int j = 0; j < size.first; j++) {
          std::cout << bigOh.getCell(std::pair<int, int>(j, i)) << " ";
        }
    }

    // Vec2d size{10, 10};
    HexGrid testGrid(size, 0);
    std::vector<std::unique_ptr<Algorithm>> algorithms;
    algorithms.emplace_back(std::make_unique<Dijkstras>(Vec2d{0, 0}, Vec2d{size.first - 1, size.second -1}, &testGrid));
    algorithms.emplace_back(std::make_unique<AStar>(Vec2d{0, 0}, Vec2d{size.first - 1, size.second -1}, &testGrid));

    std::vector<std::vector<Vec2d>> foundPaths;
    for (std::unique_ptr<Algorithm>& algo : algorithms)
    {
        foundPaths.emplace_back(algo->runAlgorithm());

    }

}
