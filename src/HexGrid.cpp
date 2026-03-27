#include "HexGrid.hpp"

#include <algorithm>
#include <queue>
#include <array>
#include <iostream>
#include <ostream>
#include <random>
#include <utility>

HexGrid::HexGrid(std::pair<int, int> big, std::pair<int, int> s, std::pair<int, int> e) : size(std::move(big)), start(std::move(s)), end(std::move(e)) {
    start = (start.first < 0 || start.second < 0) ? (std::pair<int,int>{0, size.second - 1}) : start;
    end = (end.first < 0 || end.second < 0) ? (std::pair<int,int>{size.first - 1, 0}) : end;

    grid.resize(size.second, std::vector<bool>(size.first));
    grid[start.second][start.first] = true;
    grid[end.second][end.first] = true;

    const int numCells = size.first * size.second;

    int placed = 0;

    std::random_device dev;
    std::mt19937 rng(dev());

    std::vector<int> grid1D = {};
    for (int i = 1; i < numCells - 1; i++) {
        grid1D.emplace_back(i);
    }
    while (!checkValid()) {
        std::uniform_int_distribution<std::mt19937::result_type> randCoord(1, numCells - 2 - placed);
        int index = randCoord(rng);
        int coord = grid1D[index];

        int x = coord % size.first;
        int y = coord / size.first;
        grid[y][x] = true;

        placed++;

        std::swap(grid1D[index], grid1D.back());
        grid1D.pop_back();

    }
}

// Literally just a bfs, based on discussion/lecture slides
bool HexGrid::checkValid() {
    std::queue<std::pair<int, int>> q;
    std::vector<std::vector<bool>> visited(size.second, std::vector<bool>(size.first, false));
    q.push(start);
    visited[start.second][start.first] = true;

    while (!q.empty()) {
        std::pair<int, int> current = q.front();
        q.pop();

        if (current == end) return true;

        for (std::pair<int, int> neighbor : getNeighbors(current)) {
            if (!visited[neighbor.second][neighbor.first] && grid[neighbor.second][neighbor.first]) {
                q.push(neighbor);
                visited[neighbor.second][neighbor.first] = true;
            }
        }
    }
    return false;
}

std::vector<std::pair<int, int>> HexGrid::getNeighbors(const std::pair<int, int> &cell) const {
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
