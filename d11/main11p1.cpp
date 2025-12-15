#include <format>
#include <fstream>
#include <map>
#include <print>
#include <ranges>
#include <set>
#include <string>
#include <vector>

#include <lexy/action/parse.hpp>
#include <lexy/callback.hpp>
#include <lexy/dsl.hpp>
#include <lexy/input/string_input.hpp>
#include <lexy_ext/report_error.hpp>

namespace ast {
struct server {
    std::string from;
    std::vector<std::string> to;
};
} // namespace ast

template<>
struct std::formatter<ast::server> : std::formatter<char> {
    template<typename FormatCtx>
    auto format(const ast::server &s, FormatCtx &ctx) const {
        return std::format_to(ctx.out(), "{{ from: {}, to: {} }}", s.from, s.to);
    }
};

namespace grammar {
namespace dsl = lexy::dsl;

struct id {
    static constexpr auto rule = dsl::identifier(dsl::ascii::alpha);
    static constexpr auto value = lexy::as_string<std::string>;
};

struct to {
    static constexpr auto rule = dsl::list(dsl::p<id>);
    static constexpr auto value = lexy::as_list<std::vector<std::string>>;
};

struct server {
    static constexpr auto whitespace = dsl::ascii::blank | dsl::ascii::newline;
    static constexpr auto rule = dsl::p<id> + dsl::colon + dsl::p<to>;
    static constexpr auto value = lexy::construct<ast::server>;
};
} // namespace grammar

struct graph_struct {
    std::vector<std::string> index_to_name;
    std::map<std::string, int> name_to_index;
    std::map<int, std::vector<int>> edges;
};

template<>
struct std::formatter<graph_struct> : std::formatter<char> {
    template<typename FormatCtx>
    auto format(const graph_struct &graph, FormatCtx &ctx) const {
        bool start = true;
        auto it = ctx.out();
        for (const auto &[from, to]: graph.edges) {
            if (!start) it = std::format_to(it, "\n");
            start = false;
            it = std::format_to(it, "{}:", graph.index_to_name.at(from));
            for (const auto &neighbor: to)
                it = std::format_to(it, " {}", graph.index_to_name.at(neighbor));
        }
        return it;
    }
};

graph_struct parse_graph(std::istream &f) {
    const auto servers = [&] {
        std::vector<ast::server> tmp{};
        for (std::string line; std::getline(f, line);)
            tmp.emplace_back(lexy::parse<grammar::server>(lexy::string_input(line), lexy_ext::report_error).value());
        return tmp;
    }();

    const auto names = [&] {
        std::set<std::string> tmp{};
        for (const auto &server: servers) {
            tmp.insert(server.from);
            tmp.insert_range(server.to);
        }
        return std::vector(std::move(tmp).begin(), std::move(tmp).end());
    }();

    const auto name_to_index = [&] {
        std::map<std::string, int> tmp{};
        for (int i = 0; i < names.size(); ++i)
            tmp[names[i]] = i;
        return tmp;
    }();

    const auto edges = [&] {
        std::map<int, std::vector<int>> tmp{};
        for (const auto &server: servers) {
            tmp[name_to_index.at(server.from)] = server.to
                    | std::views::transform([&](const std::string &name) { return name_to_index.at(name); })
                    | std::ranges::to<std::vector>();
        }
        return tmp;
    }();

    return {.index_to_name = names, .name_to_index = name_to_index, .edges = edges};
}

int count_paths(const graph_struct &graph, const int from, const int to) {
    if (from == to) return 1;
    int total = 0;
    for (const auto &next: graph.edges.at(from))
        total += count_paths(graph, next, to);
    return total;
}

int main() {
    // std::ifstream f{"../../d11/sample.txt"};
    std::ifstream f{"../../d11/assignment.txt"};

    const auto graph = parse_graph(f);
    std::println("{}", graph);

    std::println("Result: {}", count_paths(graph, graph.name_to_index.at("you"), graph.name_to_index.at("out")));

    return 0;
}
