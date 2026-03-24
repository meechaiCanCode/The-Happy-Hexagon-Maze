//
// Created by bigna on 3/24/2026.
//

#include "Algorithm.hpp"

#include <chrono>

using namespace std::chrono;

void Algorithm::nextIteration()
{
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
