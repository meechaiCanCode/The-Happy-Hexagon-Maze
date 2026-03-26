#include <iostream>
#include "HexGrid.hpp"
#include <ctime>

int main() {
    /* I think the order of the main should be as follows:
     1. Boot GUI
     2. Allow user to pick grid size, start, finish, dead cell percentage, and algorithms
     3. Send info to gridTest.hpp for init
     4.
     */
    const std::clock_t start_time = clock();
    HexGrid motherload{{2, 2}, 0};
    const std::clock_t end_time = clock();
    std::cout << "Time elapsed: " << end_time - start_time << std::endl;
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            if (i % 2 == 1 && j == 0) std::cout << ' ';
            if (j == 0 && i == 0) std::cout << 's';
            else if (j == 1 && i == 1) std::cout << 'e';
            else std::cout << motherload.grid[i][j];
            std::cout << " ";
        }
        std::cout << "\n";
    }
    std::cout << "Time elapsed: " << end_time - start_time << std::endl;
    std::cout << "Hello, world!" << std::endl;
}