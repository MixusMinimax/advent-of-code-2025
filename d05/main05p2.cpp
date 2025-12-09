#include "lib06.hpp"

#include <fstream>
#include <print>

int main() {
    // std::ifstream f{"../../d05/sample.txt"};
    std::ifstream f{"../../d05/assignment.txt"};
    const long long count = std::ranges::fold_left(
            parse_ranges(f), 0ll, [](const long long a, const id_range &b) { return a + b.second - b.first + 1; });
    std::println("Result: {}", count);
    return 0;
}
