#include <algorithm>
#include <cassert>
#include <chrono>
#include <concepts>
#include <format>
#include <fstream>
#include <map>
#include <print>
#include <ranges>
#include <vector>

#include <vec.hpp>

using int2 = util::vec2<int>;

struct coord_mapping {
    std::vector<int> compact_to_actual;
    std::map<int, int> actual_to_compact;
};

template<>
struct std::formatter<coord_mapping> : std::formatter<char> {
    template<typename FormatContext>
    FormatContext::iterator format(const coord_mapping &cm, FormatContext &ctx) const {
        return std::format_to(ctx.out(), "coord_mapping {{ compact_to_actual: {}, actual_to_compact: {} }}",
                              cm.compact_to_actual, cm.actual_to_compact);
    }
};

template<std::ranges::forward_range R>
    requires std::integral<std::ranges::range_value_t<R>>
coord_mapping coordinate_mapping(const R &coords) {
    std::vector<int> used_coords = coords
            | std::views::transform([](const int val) { return std::array{val - 1, val, val + 1}; })
            | std::views::join
            | std::ranges::to<std::vector>();
    std::ranges::sort(used_coords);
    used_coords.erase(std::ranges::unique(used_coords).begin(), used_coords.end());
    std::map<int, int> reverse_mapping{};
    for (int i = 0; auto &&coord: used_coords)
        reverse_mapping[coord] = i++;
    // NRVO
    return {.compact_to_actual = used_coords, .actual_to_compact = reverse_mapping};
}

enum struct color { UNSET, RED, GREEN };

struct grid {
    int width, height;
    std::vector<color> data;

    grid(const int width, const int height) :
        width{width}, height{height}, data{static_cast<std::size_t>(width) * height} {}

    [[nodiscard]]
    color get(const int x, const int y) const {
        return data[idx(x, y)];
    }

    void set(const int x, const int y, const color val) { data[idx(x, y)] = val; }

private:
    constexpr int idx(const int x, const int y) const {
        assert(x >= 0);
        assert(x < width);
        assert(y >= 0);
        assert(y < height);
        return y * width + x;
    }
};

template<>
struct std::formatter<grid> : std::formatter<char> {
    template<typename FormatContext>
    static auto format(const grid &g, FormatContext &ctx) {
        std::stringstream ss{};
        for (int y = 0; y < g.height; ++y) {
            if (y != 0) ss << std::endl;
            for (int x = 0; x < g.width; ++x) {
                switch (g.get(x, y)) {
                    case color::UNSET:
                        ss << '.';
                        break;
                    case color::RED:
                        ss << '#';
                        break;
                    case color::GREEN:
                        ss << 'X';
                        break;
                }
            }
        }
        return std::ranges::copy(std::move(ss).str(), ctx.out()).out;
    }
};

int main() {

    std::ifstream f{"../../d09/sample.txt"};
    // std::ifstream f{"../../d09/assignment.txt"};
    std::vector<int2> coords{};
    int a, b;
    char comma;
    while (f >> a >> comma >> b)
        coords.emplace_back(a, b);
    const coord_mapping x_mapping = coordinate_mapping(coords | std::views::transform(&int2::x));
    const coord_mapping y_mapping = coordinate_mapping(coords | std::views::transform(&int2::y));

    const std::vector<int2> compact_coords
            = coords
            | std::views::transform([&](const int2 &c) {
                  return int2{.x = x_mapping.actual_to_compact.at(c.x), .y = y_mapping.actual_to_compact.at(c.y)};
              })
            | std::ranges::to<std::vector>();

    grid g(x_mapping.compact_to_actual.size(), y_mapping.compact_to_actual.size());

    for (const auto &[x, y]: compact_coords)
        g.set(x, y, color::RED);

    std::println("{}", g);

    return 0;
}
