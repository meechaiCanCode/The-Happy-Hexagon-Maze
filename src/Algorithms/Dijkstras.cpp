//
// Created by bigna on 3/24/2026.
//

#include "Dijkstras.hpp"

#include <iostream>

void Dijkstras::runNextIteration_()
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
            candidateCells_.emplace(neighbor, currentCell.pos, currentCell.weight + 1);
    }
}

void Dijkstras::traceFoundPath()
{
    Vec2d currentCellPos = end_;
    foundPath_.emplace({end_});
    while (currentCellPos != start_)
    {
        currentCellPos = visitedCells_.at(currentCellPos);
        foundPath_.value().emplace_back(currentCellPos);
    }
}

Dijkstras::Dijkstras(Vec2d start, Vec2d end, HexGrid* grid) : Algorithm(start, end, grid)
{
    candidateCells_.emplace(start, Vec2d{-1, -1}, 0); // The parent of the start should never be accessed
}

std::vector<Vec2d> Dijkstras::getSearchedNodes() const
{
    std::vector<Vec2d> searchedNodes;
    searchedNodes.reserve(visitedCells_.size());
    for (auto cellPair : visitedCells_)
    {
        searchedNodes.emplace_back(cellPair.first);
    }
    return searchedNodes;
}
