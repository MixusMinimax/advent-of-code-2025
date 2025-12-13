#include <format>
#include <fstream>
#include <print>
#include <string>
#include <variant>
#include <vector>

#include <lexy/action/parse.hpp>
#include <lexy/action/validate.hpp>
#include <lexy/callback.hpp>
#include <lexy/dsl.hpp>
#include <lexy/input/string_input.hpp>
#include <lexy_ext/report_error.hpp>

namespace ast {
enum struct indicator_light { OFF, ON };
}

template<>
struct std::formatter<ast::indicator_light> : std::formatter<char> {
    template<typename FormatCtx>
    auto format(const ast::indicator_light il, FormatCtx &ctx) const {
        return std::format_to(ctx.out(), "{}", il == ast::indicator_light::ON ? "#" : ".");
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
} // namespace grammar

int main() {
    auto result = lexy::parse<grammar::indicator_lights>(lexy::zstring_input("[.#.#]"), lexy_ext::report_error).value();
    std::println("{}", result);
    // std::ifstream f{"../../d10/sample.txt"};
    // // std::ifstream f{"../../d10/assignment.txt"};
    // for (std::string line; std::getline(f, line);) {
    //     auto input = lexy::string_input(line);
    //     auto result = lexy::parse<grammar::indicator_lights>(input, lexy_ext::report_error).value();
    //     std::println("{}", result);
    // }
    return 0;
}
