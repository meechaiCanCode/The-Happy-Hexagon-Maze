#pragma once
#include <utility>
#include <vector>

class HexGrid {
public:
    explicit HexGrid(std::pair<int, int> size, std::pair<int, int> start = {-1, -1}, std::pair<int, int> end = {-1, -1});

    std::pair<int, int> getSize() {return dims;}
    std::pair<int, int> getStart() {return start;}
    std::pair<int, int> getEnd() {return end;}

    bool getCell(const std::pair<int, int> &cell) {return grid.at(cell.second).at(cell.first);}
    [[nodiscard]] std::vector<std::pair<int, int>> getNeighbors(const std::pair<int, int> &cell) const;

    std::vector<std::vector<bool>> grid;
private:
    // (x,y) coordinates for each pair
    // cell being false denotes being inaccessible, true means accessible
    // std::vector<std::vector<bool>> grid;
    std::pair<int, int> dims;
    std::pair<int, int> start;
    std::pair<int, int> end;

    bool checkValid();
};
