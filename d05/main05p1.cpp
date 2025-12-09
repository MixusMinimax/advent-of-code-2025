#include "lib06.hpp"

#include <fstream>
#include <print>
#include <utility>
#include <vector>

int main() {
    // std::ifstream f{"../../d05/sample.txt"};
    std::ifstream f{"../../d05/assignment.txt"};
    const auto ranges = parse_ranges(f);
    for (const auto [fst, snd]: ranges) {
        std::println("{}-{}", fst, snd);
    }
    auto fresh_count{0};
    for (long long id; f >> id;) {
        const auto fresh = includes_id(ranges, id);
        if (fresh) ++fresh_count;
        std::println("id {} is {}.", id, fresh ? "fresh" : "spoiled");
    }
    std::println("Result: {}", fresh_count);
    return 0;
}
