cmake_minimum_required(VERSION 4.0.1)
project(The_Happy_Hexagon_Maze)

set(CMAKE_CXX_STANDARD 20)

Include(FetchContent)
FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG        v3.5.2 # seems to work with CLion - if you run into problems, try an older version of catch2 v3
)
FetchContent_MakeAvailable(Catch2)
FetchContent_Declare(SFML
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG 3.0.2
        GIT_SHALLOW ON
        EXCLUDE_FROM_ALL
        SYSTEM)
FetchContent_MakeAvailable(SFML)

add_executable(The_Happy_Hexagon_Maze
        src/main.cpp
        src/HexGrid.cpp
        src/HexGrid.hpp
        src/Algorithms/Algorithm.cpp
        src/Algorithms/Algorithm.hpp
        src/Algorithms/Dijkstras.cpp
        src/Algorithms/Dijkstras.hpp
        src/Algorithms/AStar.cpp
        src/Algorithms/AStar.hpp
)

add_executable(Tests
        test/gridTest.cpp
        src/HexGrid.cpp
        src/HexGrid.hpp
        src/Algorithms/Algorithm.cpp
        src/Algorithms/Algorithm.hpp
        src/Algorithms/Dijkstras.cpp
        src/Algorithms/Dijkstras.hpp
        src/Algorithms/AStar.cpp
        src/Algorithms/AStar.hpp
)

target_link_libraries(Tests PRIVATE Catch2::Catch2WithMain)
target_link_libraries(The_Happy_Hexagon_Maze PRIVATE SFML::Graphics)
target_compile_definitions(Tests PRIVATE _GLIBCXX_DEBUG)