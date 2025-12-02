#include <iostream>
#include <fstream>
#include <cstdint>
#include <format>

bool id_is_valid(const std::string_view id) {
    const auto l = id.length();
    if (l == 0) return false;
    if (l == 1) return true;
    if (l == 2) return id[0] != id[1];
    if (l == 3) return id[0] != id[1] || id[0] != id[2];
    for (auto test_length{1}; test_length <= l / 2; ++test_length) {
        if (l % test_length != 0) continue;
        const std::string_view first_segment = id.substr(0, test_length);
        for (auto offset{test_length}; offset < l; offset += test_length)
            if (first_segment != id.substr(offset, test_length))
                goto outer;
        return false;
    outer:;
    }
    return true;
}

int main() {
    // std::ifstream f{"../../d02/sample.txt"};
    std::ifstream f{"../../d02/assignment.txt"};
    std::uint64_t sum{0};
    std::uint64_t a, b;
    char dash, comma;
    while (f >> a >> dash >> b) {
        for (auto i{a}; i <= b; ++i)
            if (!id_is_valid(std::to_string(i))) sum += i;
        f >> comma;
    }
    std::cout << "Result: " << sum << std::endl;
    return 0;
}
