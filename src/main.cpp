#include <iostream>
#include <unordered_set>
#include "HexGrid.hpp"
#include <ctime>
#include <memory>

#include "Algorithms/Algorithm.hpp"
#include "Algorithms/Dijkstras.hpp"

int main() {
    /* I think the order of the main should be as follows:
     1. Boot GUI
     2. Allow user to pick grid size, start, finish, dead cell percentage, and algorithms
     3. Send info to gridTest.hpp for init
     4.
     */
    const std::clock_t start_time = clock();
    std::pair<int, int> size = {4, 3};
    HexGrid motherload{size, 0};
    const std::clock_t end_time = clock();
    std::cout << "Time elapsed: " << end_time - start_time << std::endl;
    for (int i = 0; i < size.second; i++) {
        for (int j = 0; j < size.first; j++) {
            if (i % 2 == 1 && j == 0) std::cout << ' ';
            if (j == 0 && i == 0) std::cout << 's';
            else if (j == size.first - 1 && i == size.second - 1) std::cout << 'e';
            else std::cout << motherload.grid[i][j];
            std::cout << " ";
        }
        std::cout << "\n";
    }
    std::cout << "Time elapsed: " << end_time - start_time << std::endl;
    std::cout << "Hello, world!" << std::endl;

    // Vec2d size{10, 10};
    HexGrid testGrid(size, 0);
    std::vector<std::unique_ptr<Algorithm>> algorithms;
    algorithms.emplace_back(std::make_unique<Dijkstras>(Vec2d{0, 0}, Vec2d{size.first - 1, size.second -1}, &testGrid));
    for (std::unique_ptr<Algorithm>& algo : algorithms)
    {
        auto john = algo->runAlgorithm();

    }

}
