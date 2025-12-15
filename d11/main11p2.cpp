#include <fstream>
#include <map>
#include <print>
#include <string>
#include <tuple>
#include <vector>

#include "model.h"

long long count_paths(std::map<std::tuple<int, int, int, int, bool, bool>, long long> &cache, const graph_struct &graph,
                      const int from, const int to, const int find_a, const int find_b, bool found_a = false,
                      bool found_b = false) {
    const std::tuple key{from, to, find_a, find_b, !!found_a, !!found_b};
    if (const auto v = cache.find(key); v != cache.end()) return v->second;
    if (from == find_a) found_a = true;
    if (from == find_b) found_b = true;
    if (from == to) return found_a && found_b ? 1 : 0;
    long long total = 0;
    for (const auto &next: graph.edges.at(from))
        total += count_paths(cache, graph, next, to, find_a, find_b, found_a, found_b);
    cache[key] = total;
    return total;
}

int main() {
    // std::ifstream f{"../../d11/sample2.txt"};
    std::ifstream f{"../../d11/assignment.txt"};

    const auto graph = parse_graph(f);
    std::println("{}", graph);

    const int from = graph.name_to_index.at("svr");
    const int to = graph.name_to_index.at("out");
    const int dac = graph.name_to_index.at("dac");
    const int fft = graph.name_to_index.at("fft");

    std::map<std::tuple<int, int, int, int, bool, bool>, long long> cache{};
    const long long count = count_paths(cache, graph, from, to, dac, fft);

    std::println("Result: {}", count);

    return 0;
}
