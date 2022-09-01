#include <iostream>
#include "Vector.h"

int main() {
    Vector<int> V{1,2,3,4,5,6};
    std::cout << V << std::endl;
    Vector<int> V2(V, 0, V.size() / 2, 1);
    std::cout << V << std::endl;
    std::cout << V2 << std::endl;
    Vector<int> V3;
    V3 = std::move(V);
    std::cout << V3 << std::endl;
    std::cout << V << std::endl;
    std::cout << V3.clear() << std::endl;
    std::cout << V2.resize(4,5) << std::endl;
    std::cout << V2.subVector(1, 3) << std::endl;
    std::cout << V2.size() << std::endl;
    std::cout << V2 << std::endl;
    std::cout << V2.insert(3, 4, 5) << std::endl;
    std::cout << V2.resize(2) << std::endl;
    std::cout << V2.shift(4,7) << std::endl;
    Vector<int> V4(4,5);
    std::cout << V4 << std::endl;
}