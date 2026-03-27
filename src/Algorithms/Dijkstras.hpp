//
// Created by bigna on 3/24/2026.
//

#pragma once
#include <queue>
#include <unordered_map>
#include "Algorithm.hpp"

class Dijkstras : public Algorithm
{
    struct CandidateCell
    {
        Vec2d pos;
        Vec2d parent;
        int weight;

        bool operator>(const CandidateCell& other) const
        {
            return weight > other.weight;
        }
    };
    // Maps visited cells to their parent cell
    std::unordered_map<Vec2d, Vec2d> visitedCells_;

    std::priority_queue<
        CandidateCell,
        std::vector<CandidateCell>,
        std::greater<>
    > candidateCells_;

    void runNextIteration_() override;
    // Assumes that visitedCells is populated with a path back to the start position
    void traceFoundPath();
public:
    Dijkstras(Vec2d start, Vec2d end, HexGrid* grid);
    [[nodiscard]] std::vector<Vec2d> getSearchedNodes() const override;
};
