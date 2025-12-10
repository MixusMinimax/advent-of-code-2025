#include <fstream>
#include <print>
#include <vector>

using int2 = std::pair<int, int>;

int main() {
    // std::ifstream f{"../../d09/sample.txt"};
    std::ifstream f{"../../d09/assignment.txt"};
    std::vector<int2> coords{};
    int a, b;
    char comma;
    while (f >> a >> comma >> b)
        coords.emplace_back(a, b);
    long long largest_area = -1;
    for (int i = 0; i < coords.size() - 1; ++i) {
        for (int j = i + 1; j < coords.size(); ++j) {
            const auto area = static_cast<long long>(std::abs(coords[j].first - coords[i].first) + 1)
                    * static_cast<long long>(std::abs(coords[j].second - coords[i].second) + 1);
            if (area > largest_area) largest_area = area;
        }
    }
    std::println("Result: {}", largest_area);
    return 0;
}
