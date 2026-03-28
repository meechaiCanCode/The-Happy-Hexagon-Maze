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
    std::pair<int, int> size = {40,25};
    const std::clock_t start_time = clock();
    HexGrid bigOh{size};
    const std::clock_t end_time = clock();
    for (int i = 0; i < size.second; i++) {
        std::cout << "\n";
        if (i % 2 == 1) std::cout << " ";
        for (int j = 0; j < size.first; j++) {
          std::cout << bigOh.getCell(std::pair<int, int>(j, i)) << " ";
        }
    }
}
