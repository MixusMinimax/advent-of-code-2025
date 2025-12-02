#include <iostream>
#include <fstream>
#include <cstdint>
#include <format>

bool id_is_valid(const std::string_view id) {
    if (const auto l = id.length(); l % 2 == 0 && id.substr(0, l / 2) == id.substr(l / 2))
        return false;
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
