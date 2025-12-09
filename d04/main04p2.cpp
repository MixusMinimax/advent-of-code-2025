#include <fstream>
#include <print>
#include <string>
#include <vector>

static constexpr char paper_roll = '@';

struct grid_struct {
    int width;
    int height;
    std::vector<bool> data;

    void set(const int row, const int col, const int val) {
        if (row < 0 || row >= height || col < 0 || col >= width) throw std::out_of_range("index out of range");
        data.at(row * width + col) = val;
    }

    [[nodiscard]] bool get(const int row, const int col) const {
        if (row < 0 || row >= height || col < 0 || col >= width) return false;
        return data[row * width + col];
    }
};

grid_struct parse(std::istream &f) {
    grid_struct result{.width = -1};
    std::string line;
    while (std::getline(f, line)) {
        if (result.width == -1) result.width = static_cast<int>(line.length());
        for (const char c: line)
            result.data.push_back(c == paper_roll);
    }
    result.height = static_cast<int>(result.data.size() / result.width);
    return result;
}

int neighbor_count(const grid_struct &grid, int row, int col) {
    auto count{0};
    for (auto row_off{-1}; row_off <= 1; ++row_off) {
        for (auto col_off{-1}; col_off <= 1; ++col_off) {
            if (!row_off && !col_off) continue;
            if (grid.get(row + row_off, col + col_off)) ++count;
        }
    }
    return count;
}

int main() {
    // std::ifstream f{"../../d04/sample.txt"};
    std::ifstream f{"../../d04/assignment.txt"};
    grid_struct grid = parse(f);
    bool changed{true};
    auto removed_count{0};
    while (changed) {
        changed = false;
        for (auto row{0}; row < grid.height; ++row) {
            for (auto col{0}; col < grid.width; ++col) {
                if (!grid.get(row, col)) continue;
                if (neighbor_count(grid, row, col) < 4) {
                    changed = true;
                    grid.set(row, col, false);
                    ++removed_count;
                }
            }
        }
    }
    std::println("Removed {} rolls.", removed_count);
    return 0;
}
