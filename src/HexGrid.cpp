#include "HexGrid.hpp"
#include <iostream>
#include <array>
#include <random>

HexGrid::HexGrid(std::pair<int, int> size, float deadPerc, std::pair<int, int> start, std::pair<int, int> end) {
    this->size = size;
    this->start = (start.first < 0 || start.second < 0) ? (std::pair<int,int>{0, 0}) : start;
    this->end = (end.first < 0 || end.second < 0) ? (std::pair<int,int>{0, 0}) : end;

    grid.resize(size.second, std::vector<bool>(size.first, true));

    // deadPerc cannot exceed certain values, depending on size of grid, etc.
    // maybe need to take this into account
    int numDead = deadPerc * size.first * size.second;

    // When adding mines, add, check within add, then check if there are any
    for (int i = 0; i < numDead; i++) {
        addMine();
    }
}

void HexGrid::addMine() {
    bool placed = false;

    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> randx(0, size.first - 1);
    std::uniform_int_distribution<std::mt19937::result_type> randy(0, size.second - 1);

    while (!placed) {
        int x = randx(rng);
        int y = randy(rng);

        if (x != this->start.first && y != this->start.second && x != this->end.first && y != this->end.second && grid[y][x]) {
            grid[y][x] = false;
            //if (checkValid()) placed = true;
            placed = true;
        }
    }
}

// implement
bool HexGrid::checkValid() {
    return true;
}

std::vector<std::pair<int, int>> HexGrid::getNeighbors(std::pair<int, int> cell) {
    // Assuming Grid Layout s.t. (0,0) has no bottom left, horizontally jagged. Shown in Zhu's GUI.
    // Neighbor offsets are dependent on if the row is even or odd
    const std::array<std::pair<int,int>, 6> NEIGHBOR_OFFSETS = cell.second % 2 == 0
        ? std::array<std::pair<int,int>, 6>{{
            {-1, -1}, {0, -1},
            {-1,  0}, {1,  0},
            {-1,  1}, {0,  1}
        }}
        : std::array<std::pair<int,int>, 6>{{
            {0, -1}, {1, -1},
            {-1, 0}, {1,  0},
            {0,  1}, {1,  1}
        }};

    std::vector<std::pair<int, int>> neighbors;
    for (std::pair<int, int> offset : NEIGHBOR_OFFSETS) {
        int x = cell.first + offset.first;
        int y = cell.second + offset.second;
        if (!(x < 0 || x >= size.first || y < 0 || y >= size.second)) {
            neighbors.emplace_back(x, y);
        }
    }
    return neighbors;
}
