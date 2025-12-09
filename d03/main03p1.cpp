#include <fstream>
#include <print>

int main() {
    // std::ifstream f{"../../d03/sample.txt"};
    std::ifstream f{"../../d03/assignment.txt"};
    f >> std::noskipws;
    char c;
    auto sum{0};
    while (f) {
        auto i10{-1}, i1{-1};
        auto v10{-1}, v1{-1};
        for (auto i{0}; f >> c; ++i) {
            if (c == '\n' || c == '\r') {
                if (f.peek() == '\n') f.ignore(1);
                break;
            }
            if (v1 > v10) {
                v10 = v1;
                i10 = i1;
                v1 = -1;
            }
            if (const int v = c - '0'; v > v1) {
                v1 = v;
                i1 = i;
            }
        }
        if (i10 == -1 || i1 == -1) continue;
        sum += v10 * 10 + v1;
    }
    std::println("Result: {}", sum);
    return 0;
}
