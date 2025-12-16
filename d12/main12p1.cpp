#include <vec.hpp>

#include <format>
#include <generator>
#include <print>
#include <ranges>
#include <unordered_map>
#include <vector>

#include <lexy/action/parse.hpp>
#include <lexy/input/file.hpp>
#include <lexy_ext/report_error.hpp>

#include <scip/scip.h>
#include <scip/scipdefplugins.h>

#include "model.h"

struct placement {
    int p, x, y, rot;
    bool flip;
    constexpr bool operator==(const placement &) const = default;
};

template<>
struct std::formatter<placement> : std::formatter<char> {
    template<typename FormatCtx>
    auto format(const placement &p, FormatCtx &ctx) const {
        return std::format_to(ctx.out(), "{{ p:{}, x:{}, y:{}, rot:{}, flip:{} }}", p.p, p.x, p.y, p.rot, p.flip);
    }
};

constexpr int2 transform3x3(int2 loc, const int rot, const bool flip) {
    switch (rot) {
        case 0:
            break;
        case 1:
            loc = {2 - loc.y, loc.x};
            break;
        case 2:
            loc = {2 - loc.x, 2 - loc.y};
            break;
        case 3:
            loc = {loc.y, 2 - loc.x};
            break;
        default:
            throw std::invalid_argument("rot must be in {0,1,2,3}");
    }
    return flip ? int2{loc.y, loc.x} : loc;
}

static_assert(transform3x3({0, 0}, 0, false) == int2{0, 0});
static_assert(transform3x3({0, 1}, 0, false) == int2{0, 1});
static_assert(transform3x3({0, 0}, 1, false) == int2{2, 0});
static_assert(transform3x3({0, 0}, 2, false) == int2{2, 2});
static_assert(transform3x3({0, 0}, 3, false) == int2{0, 2});
static_assert(transform3x3({0, 0}, 1, true) == int2{0, 2});
static_assert(transform3x3({0, 0}, 3, true) == int2{2, 0});
static_assert(transform3x3({1, 1}, 3, true) == int2{1, 1});
static_assert(transform3x3({1, 0}, 3, false) == int2{0, 1});
static_assert(transform3x3({1, 0}, 1, false) == int2{2, 1});

std::generator<int2> covered_cells(const ast::present &present, const int x, const int y, const int rot,
                                   const bool flip) {
    const int2 offset{x, y};
    for (const int ly: {0, 1, 2}) {
        for (const int lx: {0, 1, 2}) {
            if (const int2 rotated = transform3x3({lx, ly}, rot, flip); present.at(rotated)) co_yield offset + rotated;
        }
    }
    co_return;
}

SCIP_RETCODE solve_tree(SCIP *scip, const std::vector<ast::present> &presents, int width, int height, bool &feasible,
                        std::vector<placement> &chosen_placements) {
    std::vector<placement> placements{};
    std::unordered_multimap<int, int> present_placements{};
    std::unordered_multimap<int2, int> cell_covering_placements{};
    for (int p = 0; p < presents.size(); ++p) {
        for (int y = 0; y < height - 2; ++y) {
            for (int x = 0; x < width - 2; ++x) {
                for (const auto rot: {0, 1, 2, 3}) {
                    for (const auto flip: {false, true}) {
                        const int placement_index = static_cast<int>(placements.size());
                        placements.emplace_back(p, x, y, rot, flip);
                        present_placements.emplace(p, placement_index);
                        for (const auto &covered: covered_cells(presents[p], x, y, rot, flip))
                            cell_covering_placements.insert(std::pair<const int2, int>{covered, placement_index});
                    }
                }
            }
        }
    }

    std::vector<std::string> strings{};
    strings.emplace_back(std::format("tree {}x{}", width, height));
    SCIP_CALL(SCIPcreateProbBasic(scip, strings.back().c_str()));

    SCIP_RATIONAL *zero, *one;
    SCIP_CALL(SCIPrationalCreate(&zero));
    SCIP_CALL(SCIPrationalCreate(&one));
    SCIPrationalSetFraction(zero, 0, 1);
    SCIPrationalSetFraction(one, 1, 1);

    std::vector<SCIP_VAR *> vars{};
    for (int vi = 0; vi < placements.size(); ++vi) {
        SCIP_VAR *var;
        SCIP_CALL(SCIPcreateVarBasic(scip, &var, nullptr, 0, 1, 1, SCIP_VARTYPE_INTEGER));
        SCIP_CALL(SCIPaddVar(scip, var));
        vars.push_back(var);
    }

    std::vector<SCIP_CONS *> constraints{};
    for (int p = 0; p < presents.size(); ++p) {
        SCIP_CONS *cons;
        strings.emplace_back(std::format("present {}", p));
        SCIP_CALL(SCIPcreateConsBasicLinear(scip, &cons, strings.back().c_str(), 0, nullptr, nullptr, 0.9, 1.1));
        for (auto [begin, end] = present_placements.equal_range(p);
             const auto placement: std::ranges::subrange(begin, end)) {
            SCIP_CALL(SCIPaddCoefLinear(scip, cons, vars[placement.second], 1.0));
        }
        SCIP_CALL(SCIPaddCons(scip, cons));
        constraints.push_back(cons);
    }

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            SCIP_CONS *cons;
            strings.emplace_back(std::format("cell {},{}", x, y));
            SCIP_CALL(SCIPcreateConsBasicLinear(scip, &cons, strings.back().c_str(), 0, nullptr, nullptr, -0.1, 1.1));
            for (auto [begin, end] = cell_covering_placements.equal_range(int2{x, y});
                 const auto placement: std::ranges::subrange(begin, end)) {
                SCIP_CALL(SCIPaddCoefLinear(scip, cons, vars[placement.second], 1.0));
            }
            SCIP_CALL(SCIPaddCons(scip, cons));
            constraints.push_back(cons);
        }
    }

    std::println("Used presents: {}", presents);

    SCIP_CALL(SCIPsolve(scip));

    chosen_placements = {};
    if (SCIP_SOL *sol = SCIPgetBestSol(scip); sol != nullptr) {
        std::println("Solution found.");
        for (int pl = 0; pl < placements.size(); ++pl) {
            if (const auto val = SCIPgetSolVal(scip, sol, vars[pl]); val > 0.9 && val < 1.1)
                chosen_placements.push_back(placements[pl]);
        }
        std::println("Objective = {}", SCIPgetSolOrigObj(scip, sol));
        feasible = true;
    } else {
        std::println("No solution found.");
        feasible = false;
    }
    std::println("Chosen placements: {}", chosen_placements);

    for (auto &&var: vars)
        SCIP_CALL(SCIPreleaseVar(scip, &var));
    for (auto &&cons: constraints)
        SCIP_CALL(SCIPreleaseCons(scip, &cons));
    SCIPrationalFree(&zero);
    SCIPrationalFree(&one);

    return SCIP_OKAY;
}

void place_present(std::string &out, const int width, const ast::present &p, const int x, const int y, const int rot,
                   const bool flip, const char letter) {
    const int2 offset{x, y};
    for (const int ly: {0, 1, 2}) {
        for (const int lx: {0, 1, 2}) {
            if (const auto rotated = transform3x3({lx, ly}, rot, flip); p.at(rotated)) {
                const auto pos = offset + rotated;
                if (out[pos.y * width + pos.x] != '.') {
                    std::println("pos{} is already {}", pos, out[pos.y * width + pos.x]);
                }
                out[pos.y * width + pos.x] = letter;
            }
        }
    }
}

int main() {
    const auto file = lexy::read_file<lexy::utf8_encoding>("../../d12/sample.txt");
    // const auto file = lexy::read_file<lexy::utf8_encoding>("../../d12/assignment.txt");
    const auto result = lexy::parse<grammar::file>(file.buffer(), lexy_ext::report_error).value();
    std::println("{}", result);

    Scip *scip = nullptr;
    SCIP_CALL(SCIPcreate(&scip));
    SCIP_CALL(SCIPincludeDefaultPlugins(scip));

    for (const auto &tree: result.trees) {
        bool feasible;
        std::vector<placement> chosen_placements;
        std::vector<ast::present> used_presents{};
        for (int pr = 0; pr < result.presents.size(); ++pr)
            for (int i = 0; i < tree.present_counts[pr]; ++i)
                used_presents.push_back(result.presents[pr]);
        SCIP_CALL(solve_tree(scip, used_presents, tree.w, tree.h, feasible, chosen_placements));
        std::string field(tree.w * tree.h, '.');
        for (const auto placement: chosen_placements) {
            place_present(field, tree.w, used_presents[placement.p], placement.x, placement.y, placement.rot,
                          placement.flip, used_presents[placement.p].id + '0');
        }
        for (int y = 0; y < tree.h; ++y) {
            std::println("{}", std::string_view{field}.substr(y * tree.w, tree.w));
        }
    }

    SCIP_CALL(SCIPfree(&scip));
    BMScheckEmptyMemory();

    return 0;
}
