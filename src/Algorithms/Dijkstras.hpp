//
// Created by bigna on 3/24/2026.
//

#pragma once
#include "Algorithm.hpp"


class Dijkstras : public Algorithm
{
    std::set<Vec2d>
    void runNextIteration_() override;

public:
    [[nodiscard]] std::vector<Vec2d> getSearchedNodes() const override;
    [[nodiscard]] std::optional<std::vector<Vec2d>> getFoundPath() const override;
};
