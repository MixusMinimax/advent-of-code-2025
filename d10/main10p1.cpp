#include <bit>
#include <cstdint>
#include <format>
#include <fstream>
#include <print>
#include <string>
#include <vector>

#include <lexy/action/parse.hpp>
#include <lexy/callback.hpp>
#include <lexy/dsl.hpp>
#include <lexy/input/string_input.hpp>
#include <lexy_ext/report_error.hpp>

using namespace std::literals;

namespace ast {
enum struct indicator_light { OFF, ON };

struct machine {
    std::vector<indicator_light> target;
    std::vector<std::vector<int>> buttons;
    std::vector<int> joltage_target;
};
} // namespace ast

template<>
struct std::formatter<ast::indicator_light> : std::formatter<char> {
    template<typename FormatCtx>
    auto format(const ast::indicator_light il, FormatCtx &ctx) const {
        return std::format_to(ctx.out(), "{}", il == ast::indicator_light::ON ? "#" : ".");
    }
};

template<>
struct std::formatter<ast::machine> : std::formatter<char> {
    template<typename FormatCtx>
    auto format(const ast::machine &il, FormatCtx &ctx) const {
        return std::format_to(ctx.out(), "{}, {}, {}", il.target, il.buttons, il.joltage_target);
    }
};

namespace grammar {
namespace dsl = lexy::dsl;

struct indicator_light {
    static constexpr auto table
            = lexy::symbol_table<ast::indicator_light>.map<LEXY_SYMBOL(".")>(ast::indicator_light::OFF).map<LEXY_SYMBOL("#")>(ast::indicator_light::ON);

    static constexpr auto rule = dsl::symbol<table>;
    static constexpr auto value = lexy::forward<ast::indicator_light>;
};

struct indicator_lights {
    static constexpr auto rule = dsl::square_bracketed.opt_list(dsl::p<indicator_light>);
    static constexpr auto value = lexy::as_list<std::vector<ast::indicator_light>>;
};

struct button {
    static constexpr auto rule = dsl::round_bracketed.opt_list(dsl::integer<int>, dsl::sep(dsl::comma));
    static constexpr auto value = lexy::as_list<std::vector<int>>;
};

struct buttons {
    static constexpr auto rule = dsl::list(dsl::p<button>);
    static constexpr auto value = lexy::as_list<std::vector<std::vector<int>>>;
};

struct joltage_requirements {
    static constexpr auto rule = dsl::curly_bracketed.opt_list(dsl::integer<int>, dsl::sep(dsl::comma));
    static constexpr auto value = lexy::as_list<std::vector<int>>;
};

struct machine {
    static constexpr auto whitespace = dsl::ascii::blank / dsl::ascii::newline;
    static constexpr auto rule = dsl::p<indicator_lights> + dsl::p<buttons> + dsl::p<joltage_requirements>;
    static constexpr auto value = lexy::construct<ast::machine>;
};
} // namespace grammar

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
