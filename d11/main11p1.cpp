#include <fstream>
#include <map>
#include <print>
#include <string>
#include <vector>

#include "model.h"

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
