#ifndef AOC2025_MODEL_H
#define AOC2025_MODEL_H


#include <format>
#include <vector>

#include <lexy/callback.hpp>
#include <lexy/dsl.hpp>

namespace ast {
struct present {
    int id;
    std::vector<bool> covered_cells;
};

struct tree {
    int w, h;
    std::vector<int> present_counts;
};

struct file {
    std::vector<present> presents;
    std::vector<tree> trees;
};
} // namespace ast

template<>
struct std::formatter<ast::present> : std::formatter<char> {
    template<typename FormatCtx>
    auto format(const ast::present &p, FormatCtx &ctx) const {
        return std::format_to(ctx.out(), "{}: {}", p.id, p.covered_cells);
    }
};

template<>
struct std::formatter<ast::tree> : std::formatter<char> {
    template<typename FormatCtx>
    auto format(const ast::tree &t, FormatCtx &ctx) const {
        return std::format_to(ctx.out(), "{}x{}: {}", t.w, t.h, t.present_counts);
    }
};

template<>
struct std::formatter<ast::file> : std::formatter<char> {
    template<typename FormatCtx>
    auto format(const ast::file &f, FormatCtx &ctx) const {
        auto it = ctx.out();
        it = std::format_to(it, "Presents:\n");
        for (const auto &p: f.presents)
            it = std::format_to(it, "{}\n", p);
        it = std::format_to(it, "\nTrees:\n");
        for (const auto &t: f.trees)
            it = std::format_to(it, "{}\n", t);
        return it;
    }
};

namespace grammar {
namespace dsl = lexy::dsl;

struct present_field {
    static constexpr auto table = lexy::symbol_table<bool>.map<LEXY_SYMBOL("#")>(true).map<LEXY_SYMBOL(".")>(false);
    static constexpr auto rule = dsl::symbol<table>;
    static constexpr auto value = lexy::forward<bool>;
};

struct present_body {
    static constexpr auto whitespace = dsl::ascii::blank | dsl::newline;
    static constexpr auto rule = dsl::list(dsl::p<present_field>);
    static constexpr auto value = lexy::as_list<std::vector<bool>>;
};

struct present {
    static constexpr auto whitespace = dsl::ascii::blank;
    static constexpr auto rule = dsl::integer<int> + dsl::colon + dsl::newline + dsl::p<present_body>;
    static constexpr auto value = lexy::construct<ast::present>;
};

struct presents {
    static constexpr auto whitespace = dsl::ascii::blank;
    static constexpr auto rule = dsl::list(dsl::peek(dsl::integer<int> + dsl::colon) >> dsl::p<present>);
    static constexpr auto value = lexy::as_list<std::vector<ast::present>>;
};

struct tree_area {
    static constexpr auto rule = dsl::list(dsl::integer<int>);
    static constexpr auto value = lexy::as_list<std::vector<int>>;
};

struct tree {
    static constexpr auto whitespace = dsl::ascii::blank;
    static constexpr auto rule = dsl::integer<int> + LEXY_LIT("x") + dsl::integer<int> + dsl::colon + dsl::p<tree_area>;
    static constexpr auto value = lexy::construct<ast::tree>;
};

struct trees {
    static constexpr auto rule
            = dsl::list(dsl::peek(dsl::integer<int>) >> dsl::p<tree>, dsl::trailing_sep(dsl::newline));
    static constexpr auto value = lexy::as_list<std::vector<ast::tree>>;
};

struct file {
    static constexpr auto rule = dsl::p<presents> + dsl::p<trees>;
    static constexpr auto value = lexy::construct<ast::file>;
};
} // namespace grammar

#endif // AOC2025_MODEL_H
