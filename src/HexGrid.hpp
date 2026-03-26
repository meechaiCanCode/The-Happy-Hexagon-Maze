#pragma once
#include <utility>
#include <vector>

class HexGrid {
public:
    HexGrid(std::pair<int, int> size, float deadPerc = 0.35, std::pair<int, int> start = {-1, -1}, std::pair<int, int> end = {-1, -1});

    std::pair<int, int> getSize() {return size;}
    std::pair<int, int> getStart() {return start;}
    std::pair<int, int> getEnd() {return end;}

    bool getCell(int x, int y) {return grid[y][x];}
    std::vector<std::pair<int, int>> getNeighbors(std::pair<int, int> cell);

    std::vector<std::vector<bool>> grid;
private:
    // (x,y) coordinates for each pair
    // cell being false denotes being inaccessible, true means accessible
    // std::vector<std::vector<bool>> grid;
    std::pair<int, int> size;
    std::pair<int, int> start;
    std::pair<int, int> end;
    std::vector<std::pair<int, int>> shortestPath;

    bool checkValid();
};
