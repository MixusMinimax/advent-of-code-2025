#include <bit>
#include <cstdint>
#include <format>
#include <fstream>
#include <iostream>
#include <print>
#include <string>
#include <vector>

#include <lexy/action/parse.hpp>
#include <lexy/input/string_input.hpp>
#include <lexy_ext/report_error.hpp>

#include <scip/scip.h>
#include <scip/scipdefplugins.h>

#include "model.hpp"

SCIP_RETCODE solve_machine(SCIP *scip, const ast::machine &machine, int &push_count) {
    SCIP_CALL(SCIPcreateProbBasic(scip, "machine"));

    std::vector<SCIP_VAR *> vars{};
    for (int i = 0; i < machine.buttons.size(); ++i) {
        SCIP_VAR *var;
        SCIP_CALL(SCIPcreateVarBasic(scip, &var, nullptr, 0, SCIPinfinity(scip), 1, SCIP_VARTYPE_INTEGER));
        SCIP_CALL(SCIPaddVar(scip, var));
        vars.push_back(var);
    }

    std::vector<SCIP_CONS *> constraints{};
    std::vector<std::string> constraint_names{};
    for (int i = 0; i < machine.joltage_target.size(); ++i) {
        SCIP_CONS *cons;
        const auto target = machine.joltage_target[i];
        constraint_names.push_back(std::format("const{}", i));
        SCIP_CALL(SCIPcreateConsBasicLinear(scip, &cons, constraint_names.back().c_str(), 0, nullptr, nullptr, target,
                                            target));
        for (int b = 0; b < machine.buttons.size(); ++b) {
            if (std::ranges::find(machine.buttons[b], i) != machine.buttons[b].end()) {
                std::println("Button at {} affects {} to become {}", b, i, target);
                SCIP_CALL(SCIPaddCoefLinear(scip, cons, vars[b], 1.0));
            }
        }
        SCIP_CALL(SCIPaddCons(scip, cons));
        constraints.push_back(cons);
    }

    SCIP_CALL(SCIPsolve(scip));

    if (SCIP_SOL *sol = SCIPgetBestSol(scip); sol != nullptr) {
        std::cout << "Solution found:\n";
        for (int i = 0; i < vars.size(); ++i) {
            std::print("b{} = {}, ", i, SCIPgetSolVal(scip, sol, vars[i]));
        }
        std::println("Objective = {}", push_count = SCIPgetSolOrigObj(scip, sol));
    } else {
        std::cout << "No solution found.\n";
    }

    for (auto &&var: vars)
        SCIP_CALL(SCIPreleaseVar(scip, &var));
    for (auto &&cons: constraints)
        SCIP_CALL(SCIPreleaseCons(scip, &cons));
    return SCIP_OKAY;
}

int main() {
    SCIP *scip;
    SCIP_CALL(SCIPcreate(&scip));
    SCIP_CALL(SCIPincludeDefaultPlugins(scip));

    // std::ifstream f{"../../d10/sample.txt"};
    std::ifstream f{"../../d10/assignment.txt"};
    int total = 0;
    for (std::string line; std::getline(f, line);) {
        const auto result = lexy::parse<grammar::machine>(lexy::string_input(line), lexy_ext::report_error).value();
        std::println("{}", result);
        int push_count;
        SCIP_CALL(solve_machine(scip, result, push_count));
        total += push_count;
    }

    SCIP_CALL(SCIPfree(&scip));
    BMScheckEmptyMemory();

    std::println("Result: {}", total);
    return 0;
}
