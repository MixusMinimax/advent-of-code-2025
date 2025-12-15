#ifndef MODEL_HPP
#define MODEL_HPP

#include <format>
#include <vector>

#include <lexy/callback.hpp>
#include <lexy/dsl.hpp>

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

#endif // MODEL_HPP
