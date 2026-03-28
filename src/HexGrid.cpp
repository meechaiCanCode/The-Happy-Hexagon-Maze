#include "HexGrid.hpp"

#include <algorithm>
#include <queue>
#include <array>
#include <iostream>
#include <ostream>
#include <random>
#include <utility>

HexGrid::HexGrid(std::pair<int, int> d, std::pair<int, int> s, std::pair<int, int> e) : dims(std::move(d)), start(std::move(s)), end(std::move(e)) {
    start = (start.first < 0 || start.second < 0) ? (std::pair<int,int>{0, dims.second - 1}) : start;
    end = (end.first < 0 || end.second < 0) ? (std::pair<int,int>{dims.first - 1, 0}) : end;

    grid.resize(dims.second, std::vector<bool>(dims.first));
    grid[start.second][start.first] = true;
    grid[end.second][end.first] = true;

    const int numCells = dims.first * dims.second;

    int placed = 0;

    std::random_device dev;
    std::mt19937 rng(dev());

    std::vector<int> grid1D = {};
    for (int i = 0; i < numCells; i++) {
        grid1D.emplace_back(i);
    }

    if (end.second * dims.first + end.first > start.second * dims.first + start.first) {
        std::swap(grid1D[end.second * dims.first + end.first], grid1D.back());
        grid1D.pop_back();
        std::swap(grid1D[start.second * dims.first + start.first], grid1D.back());
    }
    else {
        std::swap(grid1D[start.second * dims.first + start.first], grid1D.back());
        grid1D.pop_back();
        std::swap(grid1D[end.second * dims.first + end.first], grid1D.back());
    }
    grid1D.pop_back();

    while (!checkValid()) {
        std::uniform_int_distribution<std::mt19937::result_type> randCoord(0, numCells - 3 - placed);
        int index = randCoord(rng);

        int coord = grid1D[index];
        int x = coord % dims.first;
        int y = coord / dims.first;

        grid[y][x] = true;

        placed++;

        std::swap(grid1D[index], grid1D.back());
        grid1D.pop_back();

    }
}

// Literally just a bfs, based on discussion/lecture slides
bool HexGrid::checkValid() {
    std::queue<std::pair<int, int>> q;
    std::vector visited(dims.second, std::vector(dims.first, false));
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
        if (!(x < 0 || x >= dims.first || y < 0 || y >= dims.second)) {
            neighbors.emplace_back(x, y);
        }
    }
    return neighbors;
}
