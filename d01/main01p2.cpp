#include <iostream>
#include <fstream>
#include <format>

int main() {
    // std::ifstream f{"../../d01/sample.txt"};
    std::ifstream f{"../../d01/assignment.txt"};
    int pos{50}, count{0};
    char dir;
    int amount;
    while (f >> dir >> amount) {
        const auto full_rotations = amount / 100;
        const auto remainder = amount % 100;
        count += full_rotations;
        if (dir == 'R' && remainder >= 100 - pos || pos != 0 && dir == 'L' && remainder >= pos) ++count;
        pos = (pos + (dir == 'R' ? amount : -amount) % 100 + 100) % 100;
    }
    std::cout << "Result: " << count << std::endl;
    return 0;
}
