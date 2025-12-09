#ifndef LIB06_HPP
#define LIB06_HPP

#include <algorithm>
#include <fstream>
#include <ranges>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

using id_range = std::pair<long long, long long>;

inline std::vector<id_range> parse_ranges(std::istream &f) {
    std::vector<id_range> ranges;
    std::string line;
    while (std::getline(f, line)) {
        if (line.empty()) break;
        std::stringstream ss{line};
        long long from, to;
        char dash;
        ss >> from >> dash >> to;
        ranges.emplace_back(from, to);
    }
    std::ranges::sort(ranges);
    return ranges |
           std::ranges::views::chunk_by([](const id_range &a, const id_range &b) { return b.first <= a.second + 1; }) |
           std::ranges::views::transform([](auto &&window) {
               return id_range{std::ranges::min(window, {}, [](const id_range &range) { return range.first; }).first,
                               std::ranges::max(window, {}, [](const id_range &range) { return range.second; }).second};
           }) |
           std::ranges::to<std::vector>();
}

inline bool includes_id(const std::vector<id_range> &ranges, const long long id) {
    return std::ranges::any_of(ranges, [id](const auto &range) { return id >= range.first && id <= range.second; });
}

#endif // LIB06_HPP
