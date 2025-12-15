#include <bit>
#include <cstdint>
#include <format>
#include <fstream>
#include <print>
#include <string>
#include <vector>

#include <lexy/action/parse.hpp>
#include <lexy/input/string_input.hpp>
#include <lexy_ext/report_error.hpp>

#include "model.hpp"

ast::indicator_light toggle(const ast::indicator_light &l) {
    switch (l) {
        case ast::indicator_light::OFF:
            return ast::indicator_light::ON;
        case ast::indicator_light::ON:
            return ast::indicator_light::OFF;
    }
    throw std::invalid_argument("neither on nor off");
}

int main() {
    // std::ifstream f{"../../d10/sample.txt"};
    std::ifstream f{"../../d10/assignment.txt"};
    int total_button_presses = 0;
    for (std::string line; std::getline(f, line);) {
        const auto result = lexy::parse<grammar::machine>(lexy::string_input(line), lexy_ext::report_error).value();
        std::println("{}", result);
        const auto button_count = result.buttons.size();
        std::uint64_t smallest_mask;
        int smallest_mask_count = INT_MAX;
        for (std::uint64_t mask = 0; mask < 1ull << button_count; ++mask) {
            if (std::popcount(mask) >= smallest_mask_count) continue;
            std::vector state(result.target.size(), ast::indicator_light::OFF);
            for (int i = 0; i < button_count; ++i) {
                if (mask & 1 << i) {
                    for (auto &&flip: result.buttons[i]) {
                        state[flip] = toggle(state[flip]);
                    }
                }
            }
            if (state != result.target) continue;
            smallest_mask = mask;
            smallest_mask_count = std::popcount(mask);
        }
        std::println("Pressed {} buttons: {:b}", smallest_mask_count, smallest_mask);
        total_button_presses += smallest_mask_count;
    }
    std::println("Result: {}", total_button_presses);
    return 0;
}
