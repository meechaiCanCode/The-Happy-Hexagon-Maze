//
// Created by bigna on 3/24/2026.
//

#include "Algorithm.hpp"

#include <chrono>

using namespace std::chrono;

std::vector<Vec2d> Algorithm::getAccessibleNeighbors(Vec2d cellPos)
{
    std::vector<Vec2d> neighbors = grid_->getNeighbors(cellPos);
    std::vector<Vec2d> accessibleNeighbors;
    accessibleNeighbors.reserve(neighbors.size());

    for (Vec2d cell : neighbors)
    {
        if (grid_->getCell(cell))
            accessibleNeighbors.emplace_back(cell);
    }
    return accessibleNeighbors;
}

void Algorithm::nextIteration()
{
    if (foundPath_.has_value())
        throw std::runtime_error("Algorithm's next iteration was invoked, but a path has already been found");

    auto startTime = system_clock::now();
    runNextIteration_();
    executionTime_ += duration_cast<milliseconds>(startTime - system_clock::now()).count();
}

std::vector<Vec2d> Algorithm::runAlgorithm()
{
    auto startTime = system_clock::now();
    std::optional<std::vector<Vec2d>> foundPath = getFoundPath();

    while (!foundPath.has_value())
    {
        runNextIteration_();
        foundPath = getFoundPath();
    }

    executionTime_ += duration_cast<milliseconds>(startTime - system_clock::now()).count();
    return foundPath.value();
}
