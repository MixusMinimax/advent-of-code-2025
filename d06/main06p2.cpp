#include <coroutine>
#include <fstream>
#include <functional>
#include <iostream>
#include <print>
#include <ranges>
#include <vector>


using namespace std::string_view_literals;


int main() {
    std::ifstream f{"../../d06/sample.txt"};
    // std::ifstream f{"../../d06/assignment.txt"};
    const std::vector<std::string> lines =
            std::views::iota(0) | std::ranges::views::transform([&f](int) -> std::optional<std::string> {
                if (std::string line; std::getline(f, line)) return line;
                return std::nullopt;
            }) |
            std::views::take_while([](auto &&opt) { return opt.has_value(); }) |
            std::views::transform([](auto &&opt) { return opt.value(); }) | std::ranges::to<std::vector>();
    (void) lines;
    return 0;
}
