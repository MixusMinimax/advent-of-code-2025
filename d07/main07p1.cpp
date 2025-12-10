#include <algorithm>
#include <fstream>
#include <print>
#include <ranges>
#include <string>

int main() {
    // std::ifstream f{"../../d07/sample.txt"};
    std::ifstream f{"../../d07/assignment.txt"};
    std::string previous_line;
    std::getline(f, previous_line);
    std::ranges::replace(previous_line, 'S', '|');
    long long split_count = 0;
    for (std::string line; std::getline(f, line);) {
        for (auto i = 0; i < line.length(); ++i) {
            if (previous_line[i] == '|') {
                if (line[i] == '^') {
                    line[i-1] = '|';
                    line[i+1] = '|';
                    ++split_count;
                } else {
                    line[i] = '|';
                }
            }
        }
        previous_line = std::move(line);
    }
    std::println("Result: {}", split_count);
    return 0;
}
