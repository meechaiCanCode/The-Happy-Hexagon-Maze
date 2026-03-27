#include "AStar.hpp"
#include <cmath>


void AStar::runNextIteration_()
{
    // Get top unvisited cell
    CandidateCell currentCell;
    do
    {
        // Check if there are no more paths to check
        if (candidateCells_.empty())
        {
            foundPath_.emplace();
            return;
        }
        currentCell = candidateCells_.top();
        candidateCells_.pop();
    } while (visitedCells_.count(currentCell.pos) != 0);

    // Mark current cell as visited
    visitedCells_.emplace(currentCell.pos, currentCell.parent);

    // Check if the end has been found
    if (currentCell.pos == end_)
        return traceFoundPath();

    // Update candidates with neighboring cells
    std::vector<Vec2d> neighbors = getAccessibleNeighbors(currentCell.pos);
    for (Vec2d neighbor : neighbors)
    {
        // Current weight + the weight of the path, which is always 1
        if (visitedCells_.count(neighbor) == 0)
            candidateCells_.emplace(
                neighbor,
                currentCell.pos,
                currentCell.distFromStart + 1,
                getDistance(currentCell.pos, end_)
            );
    }
}

void AStar::traceFoundPath()
{
    Vec2d currentCellPos = end_;
    foundPath_.emplace({end_});
    while (currentCellPos != start_)
    {
        currentCellPos = visitedCells_.at(currentCellPos);
        foundPath_.value().emplace_back(currentCellPos);
    }
}

int AStar::getDistance(Vec2d a, Vec2d b)
{
    // evil cubic distance formula >:((
    // methodology from https://www.redblobgames.com/grids/hexagons/#distances
    int ax = a.first;
    int az = a.second - (a.first - (a.first & 1)) / 2; // x & 1 (bitwise and with 1) returns 0 if x is even, 1 if odd
    int ay = -ax - az;
    int bx = b.first;
    int bz = b.second - (b.first - (b.first & 1)) / 2;
    int by = -bx - bz;

    return std::max(std::max(std::abs(ax-bx), std::abs(az-bz)), std::abs(ay-by));
}

AStar::AStar(Vec2d start, Vec2d end, HexGrid* grid) : Algorithm(start, end, grid)
{
    candidateCells_.emplace(
        start, // The parent of the start should never be accessed
        Vec2d{-1, -1},
        0,
        getDistance(start, end)
        );
}

std::vector<Vec2d> AStar::getSearchedNodes() const
{
    std::vector<Vec2d> searchedNodes;
    searchedNodes.reserve(visitedCells_.size());
    for (auto cellPair : visitedCells_)
    {
        searchedNodes.emplace_back(cellPair.first);
    }
    return searchedNodes;
}
