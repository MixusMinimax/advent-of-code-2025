#include <algorithm>
#include <fstream>
#include <print>
#include <ranges>
#include <vector>


using namespace std::string_view_literals;

struct operation {
    char op;
    uint8_t width;
};

template<>
struct std::formatter<operation> : std::formatter<char> {
    template<class ParseContext>
    constexpr static ParseContext::iterator parse(ParseContext &ctx) {
        return ctx.begin();
    }

    template<class FmtContext>
    static FmtContext::iterator format(operation s, FmtContext &ctx) {
        return std::ranges::copy(std::format("{}({})", s.op, s.width), ctx.out()).out;
    }
};

long long identity(const char op) {
    if (op == '*') return 1;
    if (op == '+') return 0;
    throw std::invalid_argument(std::format("'{}' must be */+", op));
}

long long combine(const char op, const long long a, const int b) {
    if (op == '*') return a * b;
    if (op == '+') return a + b;
    throw std::invalid_argument(std::format("'{}' must be */+", op));
}

int main() {
    // std::ifstream f{"../../d06/sample.txt"};
    std::ifstream f{"../../d06/assignment.txt"};
    std::vector<std::string> lines{};
    for (std::string line; std::getline(f, line);)
        lines.emplace_back(std::move(line));
    auto ops = lines.back()
            | std::views::chunk_by([](auto, auto &&b) { return !"+*"sv.contains(b); })
            | std::views::transform([](auto &&chunk) {
                   return operation{.op = chunk[0], .width = static_cast<uint8_t>(chunk.size() - 1)};
               })
            | std::ranges::to<std::vector>();
    const size_t longest_length = std::ranges::max(lines | std::views::transform(&std::string::length));
    ops.back().width = longest_length - lines.back().length() + 1;
    lines.pop_back();
    // std::println("{}", ops);
    long long sum = 0;
    auto col = 0;
    for (const auto [op, width]: ops) {
        long long term = identity(op);
        for (auto i = 0; i < width; ++i) {
            auto number = 0;
            for (const auto &line: lines) {
                const char c = line[col];
                if (!std::isdigit(c)) continue;
                if (number) number *= 10;
                number += c - '0';
            }
            term = combine(op, term, number);
            ++col;
        }
        sum += term;
        ++col;
    }
    std::println("Result: {}", sum);
    return 0;
}
