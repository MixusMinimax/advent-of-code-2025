#include <algorithm>
#include <cassert>
#include <deque>
#include <format>
#include <fstream>
#include <map>
#include <print>
#include <ranges>
#include <sstream>
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
            | std::views::transform([](const int val) { return std::array{val, val + 1}; })
            | std::views::join
            | std::ranges::to<std::vector>();
    std::ranges::sort(used_coords);
    used_coords.erase(std::ranges::unique(used_coords).begin() - 1, used_coords.end());
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

    [[nodiscard]]
    color get(const int2 &p) const {
        return data[idx(p.x, p.y)];
    }

    void set(const int x, const int y, const color val) { data[idx(x, y)] = val; }

    void set(const int2 &p, const color val) { data[idx(p.x, p.y)] = val; }

private:
    [[nodiscard]] constexpr int idx(const int x, const int y) const {
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

/// get angular direction from vector. 0 is north, 1 is east, 2 is south, 3 is west.
/// @param vec assumed to be horizontal / vertical
/// @return [0;3]
constexpr int direction(const int2 &vec) {
    if (vec.y < 0) return 0;
    if (vec.x > 0) return 1;
    if (vec.y > 0) return 2;
    if (vec.x < 0) return 3;
    throw std::invalid_argument("[0, 0] be forbidden, me lad");
}

/// returns the relative angle. 1 to 2 is 1, 1 to 3 is 2, 1 to 3 is -1.
/// 0 means straight, 1 means right, -1 means left, 2 means 180º.
/// @param from old angle
/// @param to new anle
/// @return [-1;2]
constexpr int relative_direction(const int from, const int to) { return (to + 4 - from + 1) % 4 - 1; }

static_assert(relative_direction(0, 1) == 1);
static_assert(relative_direction(0, 2) == 2);
static_assert(relative_direction(0, 3) == -1);
static_assert(relative_direction(1, 3) == 2);
static_assert(relative_direction(3, 0) == 1);
static_assert(relative_direction(3, 1) == 2);
static_assert(relative_direction(2, 0) == 2);
static_assert(relative_direction(3, 2) == -1);
static_assert(relative_direction(3, 3) == 0);

int main() {
    // std::ifstream f{"../../d09/sample.txt"};
    std::ifstream f{"../../d09/assignment.txt"};
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
                  return int2{x_mapping.actual_to_compact.at(c.x), y_mapping.actual_to_compact.at(c.y)};
              })
            | std::ranges::to<std::vector>();

    grid g{static_cast<int>(x_mapping.compact_to_actual.size()), static_cast<int>(y_mapping.compact_to_actual.size())};

    int rot_angle = 0;
    int prev_angle = direction(compact_coords.back() - *(compact_coords.end() - 2));
    int2 prev = compact_coords.back();
    for (auto &&cur: compact_coords) {
        const auto dir = clamp(cur - prev, -1, 1);
        const auto angle = direction(dir);
        rot_angle += relative_direction(prev_angle, angle);
        prev_angle = angle;
        for (auto p = prev + dir; p != cur; p += dir)
            g.set(p, color::GREEN);
        g.set(cur, color::RED);
        prev = cur;
    }
    std::println("{}", g);
    if (rot_angle != 4 && rot_angle != -4)
        throw std::runtime_error(
                std::format("Fuck, rot_angle={}, it's not a perfect circle featuring Maynard James Keenan", rot_angle));
    const bool clockwise = rot_angle == 4;
    const auto first_dir = clamp(compact_coords[1] - compact_coords[0], -1, 1);
    const int2 right{-first_dir.y, first_dir.x};
    const auto inside = compact_coords[0] + first_dir + (clockwise ? right : -right);
    std::deque frontier{inside};
    while (!frontier.empty()) {
        const auto next = frontier.front();
        frontier.pop_front();
        if (g.get(next) != color::UNSET) continue;
        g.set(next, color::GREEN);
        frontier.append_range(std::array{next + int2{1, 0}, next + int2{0, 1}, next + int2{-1, 0}, next + int2{0, -1}});
    }

    std::pair largest_area_indices{-1, -1};
    long long largest_area = -1;
    for (int i = 0; i < coords.size() - 1; ++i) {
        for (int j = i + 1; j < coords.size(); ++j) {
            const auto a_real = coords[i];
            const auto b_real = coords[j];
            const auto area = static_cast<long long>(std::abs(b_real.x - a_real.x) + 1)
                    * static_cast<long long>(std::abs(b_real.y - a_real.y) + 1);
            if (area <= largest_area) continue;
            // check for validity
            const auto a_compact
                    = int2{x_mapping.actual_to_compact.at(a_real.x), y_mapping.actual_to_compact.at(a_real.y)};
            const auto b_compact
                    = int2{x_mapping.actual_to_compact.at(b_real.x), y_mapping.actual_to_compact.at(b_real.y)};
            const auto top_left = min(a_compact, b_compact);
            const auto bottom_right = max(a_compact, b_compact);
            for (int y = top_left.y; y <= bottom_right.y; ++y) {
                for (int x = top_left.x; x <= bottom_right.x; ++x) {
                    if (g.get(x, y) == color::UNSET) goto fail;
                }
            }
            largest_area = area;
            largest_area_indices = {i, j};
        fail:;
        }
    }

    std::println("\n{}", g);

    std::println("Largest rectangle: {} to {} with {}", coords[largest_area_indices.first],
                 coords[largest_area_indices.second], largest_area);

    return 0;
}
