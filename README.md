# Happy Hexagon Maze
## About
Race maze solving algorithms on mazes of hexagonal cells.

By: Leoan Zhu, Meechai Kerr, Nathan Almasi (AKA: The Three Hexes)
___
## Instructions
### CLion (Recommended)
1. Clone repo into CLion. Make sure that "The_Happy_Hexagon_Maze" configuration is selected.
2. Run the code in Clion using the button in the top right.
3. Select the height and width, as well as the desired searching algorithm on the input screen.
4. Enter and press play, zoom and speed up as desired.
### VSCode / Terminal
1. Clone repo.
2. In terminal, run the following commands:
    - `cd src`
    - `clang++ -std=c++20 main.cpp HexGrid.cpp Algorithms/Algorithm.cpp Algorithms/AStar.cpp Algorithms/Dijkstras.cpp -o main $(pkg-config --cflags --libs sfml-graphics sfml-window sfml-system)`
    - `./main`
3. Select the height and width, as well as the desired searching algorithm on the input screen.
4. Enter and press play, zoom and speed up as desired.
