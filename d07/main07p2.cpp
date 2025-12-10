#include <algorithm>
#include <fstream>
#include <map>
#include <print>
#include <ranges>
#include <string>
#include <vector>

static std::map<std::pair<int, int>, long long> cache{};

/*
 *
 * we're doing this with dirty, dirty recursion. However:
 * 1. Stack-wise it's fine, because the maximum depth of the stack is the number of lines in the file.
 * 2. We can prove that it terminates.
 * Both of these points are proven by the recursion: A recursion call is always done with row + 1.
 * Since the termination of recursion is based on row being bigger or equal lines.size(), which does not change,
 * we know that recursion must terminate.
 * The worse case run-time is O(2^n), which is horrible, because at every layer, we could double the recursion width.
 * In practice, since there is a finite amount of values for col, we can cache the results very well. The result of a
 * row/col pair is always the same.
 * Meaning that the amount of different possible function calls has been reduced to O(n^2 m),
 * with n being the amount of lines, and m being the length of those lines. At least I think so.
 *
 * Intuitively, we can think of this in the way that multiple paths of the beam could lead to coordinate i,j.
 * But we know that it doesn't matter where the beam came from, the amount of paths from there is the same.
 * Meaning, we only need to calculate that once.
 *
 * In practice, not including the caching resulted in execution time that outlasted my patience.
 * Adding caching cut it down to basically instantaneous.
 *
 * Converting this to iteration would be easy, but since I haven't used recursion yet in this AOC, I just felt like it.
 *
 */


long long count_paths(const std::vector<std::string> &lines, const int row, const int col) {
    if (row >= lines.size()) return 1;
    if (const auto it = cache.find({row, col}); it != cache.end()) return it->second;
    long long result;
    if (const char c = lines[row][col]; c == '^')
        result = count_paths(lines, row + 1, col - 1) + count_paths(lines, row + 1, col + 1);
    else
        result = count_paths(lines, row + 1, col);
    cache[{row, col}] = result;
    return result;
}

int main() {
    // std::ifstream f{"../../d07/sample.txt"};
    std::ifstream f{"../../d07/assignment.txt"};
    std::vector<std::string> lines{};
    for (std::string line; std::getline(f, line);)
        lines.emplace_back(std::move(line));
    const auto count = count_paths(lines, 1, static_cast<int>(lines.front().find('S')));
    std::println("Result: {}", count);
    return 0;
}
