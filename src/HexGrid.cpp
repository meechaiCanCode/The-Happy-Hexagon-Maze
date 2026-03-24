#include "HexGrid.hpp"
#include <iostream>
#include <random>

HexGrid::HexGrid(std::pair<int, int> size, float deadPerc, std::pair<int, int> start, std::pair<int, int> end) {
    this->size = size;
    this->start = (start.first < 0 || start.second < 0) ? (std::pair<int,int>{0, 0}) : start;
    this->end = (end.first < 0 || end.second < 0) ? (std::pair<int,int>{0, 0}) : end;

    grid.resize(size.second, std::vector<bool>(size.first, true));

    int numDead = deadPerc * size.first * size.second;

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
