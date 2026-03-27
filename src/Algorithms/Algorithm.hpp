#pragma once
#include <optional>
#include <utility>
#include <vector>
#include "../HexGrid.hpp"

using Vec2d = std::pair<int, int>;
template<>
struct std::hash<Vec2d>
{
    size_t operator()(const Vec2d& p) const noexcept
    {
        return hash<int>{}(p.first) ^ hash<int>{}(p.second);
    }
};

/**
 * Abstract class defining common features of an algorithm. Each iteration can be executed manually, and a running
 * timer is maintained for execution time to be compared between algorithms
 */
class Algorithm
{
private:
    long long executionTime_ = 0;
protected:
    Vec2d start_;
    Vec2d end_;

    HexGrid* grid_;

    std::optional<std::vector<Vec2d>> foundPath_ = std::nullopt;

    std::vector<Vec2d> getAccessibleNeighbors(Vec2d cellPos);

    virtual void runNextIteration_() = 0;


public:
    Algorithm(Vec2d start, Vec2d end, HexGrid* grid) : start_(std::move(start)), end_(std::move(end)), grid_(grid) {};
    virtual ~Algorithm() = default;

    /**
     * Starts the next cycle of the algorithm, useful if you want to synchronize the algorithms for the visualization.
     * Updates the running timer according to the system time before and after the algorithm was run.
     */
    void nextIteration();

    /**
     * Calls nextIteration() repeatedly until a path is found or all nodes are traversed, returning the resulting path.
     * @return The path found from the traversal. If the algorithm has traversed every possible node but not found
     * the exit point, the vector will be empty.
     */
    std::vector<Vec2d> runAlgorithm();

    /**
     * Gets a running total in ms for how much cpu time the program has run
     * @return Time in ms
     */
    [[nodiscard]] long long getExecutionTime() const { return executionTime_; };

    /**
     * Gets the path found by the algorithm
     * @return A vector containing coordinate pairs for all the nodes in the found path.
     * - If the path has not yet been
     * found, the std::optional will contain no value.
     * - If the algorithm has traversed every possible node but not found
     * the exit point, the vector will be empty.
     */
    [[nodiscard]] const std::optional<std::vector<Vec2d>>& getFoundPath() const { return foundPath_; };

    /**
     * Gets all nodes previously traversed by the algorithm
     * @return a vector containing coordinate pairs for each traversed node
     */
    [[nodiscard]] virtual std::vector<Vec2d> getSearchedNodes() const = 0;

};
