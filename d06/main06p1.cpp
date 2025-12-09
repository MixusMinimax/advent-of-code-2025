#include <fstream>
#include <print>
#include <ranges>
#include <vector>

int main() {
    // std::ifstream f{"../../d06/sample.txt"};
    std::ifstream f{"../../d06/assignment.txt"};
    const std::vector<long long> numbers = std::ranges::istream_view<long long>{f} | std::ranges::to<std::vector>();
    f.clear();
    f.unget();
    const std::vector<char> ops = std::ranges::istream_view<char>{f} | std::ranges::to<std::vector>();
    const auto width = ops.size();
    const auto height = numbers.size() / width;
    std::println("width {}, height {}", width, height);
    long long result = 0;
    for (auto col = 0; col < width; ++col) {
        long long acc{0};
        const auto op = ops[col];
        switch (op) {
            case '*':
                acc = 1;
                break;
            case '+':
                acc = 0;
                break;
            default:;
        }
        for (auto row = 0; row < height; ++row) {
            const auto idx = row * width + col;
            switch (op) {
                case '*':
                    acc *= numbers[idx];
                    break;
                case '+':
                    acc += numbers[idx];
                    break;
                default:;
            }
        }
        result += acc;
    }
    std::println("Result: {}", result);
    return 0;
}
