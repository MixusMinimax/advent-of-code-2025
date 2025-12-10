// ReSharper disable CppUseStructuredBinding
#include <algorithm>
#include <array>
#include <format>
#include <fstream>
#include <map>
#include <print>
#include <ranges>
#include <sstream>
#include <vector>

union int3 {
    struct {
        int x, y, z;
    };

    int data[3];
};

template<>
struct std::formatter<int3> : std::formatter<char> {
    template<class FmtContext>
    FmtContext::iterator format(const int3 v, FmtContext &ctx) const {
        return std::format_to(ctx.out(), "{}", v.data);
    }
};

struct box {
    int3 pos;
    int circuit_id;
};

template<>
struct std::formatter<box> : std::formatter<char> {
    template<class FmtContext>
    FmtContext::iterator format(const box v, FmtContext &ctx) const {
        return std::format_to(ctx.out(), "{{{}, {}}}", v.pos, v.circuit_id);
    }
};

struct box_pair {
    int ia, ib;
    // had to change this to double, because these numbers were getting too large even for uint64_t.
    // but a double can be basically as large as it wants to be, while losing some precision.
    // precision is not that necessary though if the distances between pairs is different enough, which is the case.
    // It's good enough for sorting. Even single precision might be enough.
    double square_dist;
};

template<>
struct std::formatter<box_pair> : std::formatter<char> {
    template<class FmtContext>
    FmtContext::iterator format(const box_pair v, FmtContext &ctx) const {
        return std::format_to(ctx.out(), "{{{}, {}, {}}}", v.ia, v.ib, v.square_dist);
    }
};

constexpr bool sample = false;

// constexpr bool sample = true;

int main() {
    std::ifstream f{sample ? "../../d08/sample.txt" : "../../d08/assignment.txt"};
    constexpr int k = sample ? 10 : 1000;
    std::vector<box> boxes;
    for (std::string line; std::getline(f, line);) {
        std::stringstream ss{std::move(line)};
        int3 pos{};
        char comma;
        ss >> pos.x >> comma >> pos.y >> comma >> pos.z;
        boxes.emplace_back(pos, -1);
    }

    // what's the worst that could happen?
    std::vector<box_pair> all_pairs{};
    for (auto i = 0; i < boxes.size() - 1; ++i) {
        const auto a = boxes[i].pos;
        for (auto j = i + 1; j < boxes.size(); ++j) {
            const auto b = boxes[j].pos;
            all_pairs.emplace_back(i, j,
                                   static_cast<double>(b.x - a.x) * (b.x - a.x)
                                           + static_cast<double>(b.y - a.y) * (b.y - a.y)
                                           + static_cast<double>(b.z - a.z) * (b.z - a.z));
        }
    }
    // the assignment asks of us to only think about the closes k pairs.
    std::ranges::partial_sort(all_pairs.begin(), all_pairs.begin() + k, all_pairs.end(), {}, &box_pair::square_dist);
    all_pairs.resize(k);

    int last_circuit_id = -1;
    std::map<int, std::vector<int>> circuits{};

    for (const auto pair: all_pairs) {
        const int ia = pair.ia;
        const int ib = pair.ib;
        if (box &box_a = boxes[ia], &box_b = boxes[ib]; box_a.circuit_id == -1 && box_b.circuit_id == -1) {
            std::println("Forming new circuit between {} and {} ({})", box_a, box_b, pair.square_dist);
            const int new_circuit_id = ++last_circuit_id;
            box_a.circuit_id = new_circuit_id;
            box_b.circuit_id = new_circuit_id;
            circuits[new_circuit_id] = std::vector{ia, ib};
        } else if (box_a.circuit_id == -1) {
            std::println("a Adding {} to {}", box_a, box_b);
            box_a.circuit_id = box_b.circuit_id;
            circuits[box_b.circuit_id].emplace_back(ia);
        } else if (box_b.circuit_id == -1) {
            std::println("b Adding {} to {}", box_b, box_a);
            box_b.circuit_id = box_a.circuit_id;
            circuits[box_a.circuit_id].emplace_back(ib);
        } else if (box_a.circuit_id != box_b.circuit_id) {
            const int new_circuit_id = box_a.circuit_id;
            const int old_circuit_id = box_b.circuit_id;
            std::println("Merging {} into {}", old_circuit_id, new_circuit_id);
            for (int b_member: circuits[old_circuit_id])
                boxes[b_member].circuit_id = new_circuit_id;
            circuits[new_circuit_id].append_range(std::move(circuits[old_circuit_id]));
            circuits.erase(old_circuit_id);
        } else {
            std::println("{} and {} already of same circuit", box_a, box_b);
        }
    }

    std::vector<size_t> sizes = circuits
            | std::views::transform([](auto &&p) { return p.second.size(); })
            | std::ranges::to<std::vector>();

    std::ranges::partial_sort(sizes.begin(), sizes.begin() + 3, sizes.end(), std::greater{});
    const auto product = std::ranges::fold_left(sizes | std::views::take(3), 1, std::multiplies{});

    std::println("Result: {}", product);

    return 0;
}
