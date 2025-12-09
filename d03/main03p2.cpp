#include <fstream>
#include <print>

long long pow10(const long long n) {
    long long r = 1;
    for (int i = 0; i < n; ++i)
        r *= 10;
    return r;
}

int main() {
    // std::ifstream f{"../../d03/sample.txt"};
    std::ifstream f{"../../d03/assignment.txt"};
    long long sum{0};
    for (std::string line; std::getline(f, line);) {
        const int len = static_cast<int>(line.length());
        long long result = 0;
        auto start_index = 0;
        for (auto digit{11}; digit >= 0; --digit) {
            const auto end_index = len - digit;
            const auto max = std::max_element(line.begin() + start_index, line.begin() + end_index);
            start_index = static_cast<int>(std::distance(line.begin(), max)) + 1;
            result += (*max - '0') * pow10(digit);
        }
        sum += result;
    }
    std::println("Result: {}", sum);
    return 0;
}
