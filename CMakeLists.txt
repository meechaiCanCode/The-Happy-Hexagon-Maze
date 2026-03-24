cmake_minimum_required(VERSION 4.1)
project(The_Happy_Hexagon_Maze)

set(CMAKE_CXX_STANDARD 14)

Include(FetchContent)
FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG        v3.5.2 # seems to work with CLion - if you run into problems, try an older version of catch2 v3
)
FetchContent_MakeAvailable(Catch2)

add_executable(The_Happy_Hexagon_Maze
        src/main.cpp
        src/HexGrid.cpp
        src/HexGrid.hpp
)

add_executable(Tests
        test/gridTest.cpp
        src/HexGrid.cpp
        src/HexGrid.hpp
)

target_link_libraries(Tests PRIVATE Catch2::Catch2WithMain)