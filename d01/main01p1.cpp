#include <iostream>
#include <fstream>

int main() {
    // std::ifstream f{"../../d01/sample.txt"};
    std::ifstream f{"../../d01/assignment.txt"};
    int pos{50}, count{0};
    char dir;
    int amount;
    while (f >> dir >> amount) {
        pos = (pos + (dir == 'R' ? amount : -amount) % 100 + 100) % 100;
        if (pos == 0) ++count;
    }
    std::cout << "Result: " << count << std::endl;
    return 0;
}
