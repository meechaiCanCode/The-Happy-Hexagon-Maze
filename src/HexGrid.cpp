#include "HexGrid.hpp"

#include <algorithm>
#include <iostream>
#include <queue>
#include <array>
#include <map>
#include<algorithm>
#include <random>

HexGrid::HexGrid(std::pair<int, int> size, float deadPerc, std::pair<int, int> start, std::pair<int, int> end) {
    this->size = size;
    this->start = (start.first < 0 || start.second < 0) ? (std::pair<int,int>{0, 0}) : start;
    this->end = (end.first < 0 || end.second < 0) ? (std::pair<int,int>{size.first - 1, size.second - 1}) : end;

    grid.resize(size.second, std::vector<bool>(size.first, true));

    int numCells = size.first * size.second;
    int numDead = numCells - shortestPath.size() > numCells * deadPerc ? numCells * deadPerc : numCells - shortestPath.size();
    int placed = 0;
    checkValid();

    std::vector<int> grid1D = {};
    for (int i = 0; i < numCells; i++) {
        grid1D.emplace_back(i);
    }

    std::random_device dev;
    std::mt19937 rng(dev());

    while (placed < numDead && placed + shortestPath.size() < numCells) {
        //if (placed % 1000 == 0) std::cout << placed << std::endl;
        std::cout << placed << ":" << numDead << ":" << shortestPath.size() << std::endl;
        std::uniform_int_distribution<std::mt19937::result_type> randCoord(1, numCells - 2);
        int index = randCoord(rng);
        int coord = grid1D[index];
        int x = coord % size.first;
        int y = coord / size.first;
        //std::cout << index << std::endl;


        grid[y][x] = false;
        if (!coordInShortestPath(std::pair<int,int>{x,y}) || checkValid()) {
            placed++;
            /*std::cout << index << std::endl;
            for (auto co : grid1D) {
                std::cout << co << ":" << "(" << x << "," << y << ")" << std::endl;
            }*/
            /*std::swap(grid1D[index], grid1D.back());
            grid1D.pop_back();*/
            grid1D.erase(std::next(grid1D.begin(), index));
            std::cout << index << "\n coord:";
            for (auto co : grid1D) {
                std::cout << co << " ";
            }
            std::cout << "\n i:    ";
            for (int g = 0; g < grid1D.size(); g++) {
                std::cout << g << " ";
            }
            std::cout << std::endl;
        }
        else {
            grid[y][x] = true;
        }
    }
}

// Literally just a bfs, based on discussion/lecture slides
bool HexGrid::checkValid() {
    //std::cout << "Running CheckValid" << std::endl;
    std::queue<std::pair<int, int>> q;
    std::vector<std::vector<bool>> visited(size.second, std::vector<bool>(size.first, false));
    std::map<std::pair<int, int>, std::pair<int, int>> parent;

    q.push(start);
    visited[start.second][start.first] = true;

    while (!q.empty()) {
        std::pair<int, int> current = q.front();
        q.pop();

        if (current == end) {
            shortestPath.clear();
            std::pair<int, int> pathNode = end;
            while (pathNode != start) {
                shortestPath.push_back(pathNode);
                pathNode = parent[pathNode];
            }
            shortestPath.push_back(start);
            std::reverse(shortestPath.begin(), shortestPath.end());
            return true;
        }

        for (std::pair<int, int> neighbor : getNeighbors(current)) {
            if (!visited[neighbor.second][neighbor.first] && grid[neighbor.second][neighbor.first]) {
                q.push(neighbor);
                visited[neighbor.second][neighbor.first] = true;
                parent[neighbor] = current;
            }
        }
    }

    return false;
}

bool HexGrid::coordInShortestPath(std::pair<int, int> coord) {
    for (auto pathCell : shortestPath) {
        if (pathCell == coord) {
            return true;
        }
    }
    return false;

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
