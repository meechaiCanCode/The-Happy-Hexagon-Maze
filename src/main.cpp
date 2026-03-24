#include <iostream>
#include "HexGrid.hpp"

int main() {
    HexGrid test({10, 10});
    int num1s = 0;
    int num0s = 0;
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            std::cout << (test.grid)[i][j] << " ";
            if ((test.grid)[i][j] == 1) num1s++;
            else num0s++;
        }
        std::cout << std::endl;
    }
    std::cout << num1s << std::endl;
    std::cout << num0s << std::endl;
}