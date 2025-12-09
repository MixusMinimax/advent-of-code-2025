#include <fstream>
#include <print>
#include <string>

static constexpr char paper_roll = '@';
static constexpr char empty_space = '.';

struct lines_struct {
    std::string previous_line{}, current_line{}, next_line{};

    void advance(std::string &next) {
        std::swap(previous_line, current_line);
        std::swap(current_line, next_line);
        std::swap(next_line, next);
    }
    void advance(std::string &&next) { advance(next); }

    [[nodiscard]] char at(const int row, const int col) const {
        if (row < -1 || row > 1) return empty_space;
        const auto &line = line_at(row);
        if (col < 0 || col >= line.length()) return empty_space;
        return line[col];
    }

    [[nodiscard]] char operator[](const std::pair<int, int> &c) const { return at(c.first, c.second); }

    /**
     * @param row in {-1, 0, 1}
     * @return lvalue reference to the chosen line
     */
    [[nodiscard]] const std::string &line_at(const int row) const {
        switch (row) {
            case -1:
                return previous_line;
            case 0:
                return current_line;
            case 1:
                return next_line;
            default:
                throw std::invalid_argument("row must be in {-1, 0, 1}");
        }
    }

    /**
     * @param row in {-1, 0, 1}
     * @return lvalue reference to the chosen line
     */
    [[nodiscard]] std::string &line_at(const int row) {
        return const_cast<std::string &>(static_cast<const lines_struct *>(this)->line_at(row));
    }

    [[nodiscard]] int width() const { return static_cast<int>(current_line.length()); }
};

int main() {
    // std::ifstream f{"../../d04/sample.txt"};
    std::ifstream f{"../../d04/assignment.txt"};
    lines_struct lines{};

    auto accessible_count{0};
    for (std::string next; std::getline(f, next) || !lines.next_line.empty();) {
        lines.advance(next); // next now holds the old data of previous_line
        if (lines.current_line.empty()) continue;
        for (auto col{0}; col < lines.width(); ++col) {
            if (lines[{0, col}] != paper_roll) continue;
            auto surrounding_count{0};
            for (auto row_off{-1}; row_off <= 1; ++row_off) {
                for (auto col_off{-1}; col_off <= 1; ++col_off) {
                    if (!row_off && !col_off) continue;
                    if (lines[{row_off, col + col_off}] == paper_roll) ++surrounding_count;
                }
            }
            if (surrounding_count < 4) ++accessible_count;
        }
    }
    std::println("Result: {}", accessible_count);
}
