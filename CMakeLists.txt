cmake_minimum_required(VERSION 4.1)
project(The_Happy_Hexagon_Maze)

set(CMAKE_CXX_STANDARD 14)

add_executable(The_Happy_Hexagon_Maze
        src/main.cpp
        src/HexGrid.cpp
        src/HexGrid.hpp
        test/gridTest.cpp
)
