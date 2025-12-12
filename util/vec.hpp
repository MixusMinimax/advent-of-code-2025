#ifndef AOC2025_VEC_H
#define AOC2025_VEC_H

#include <format>

namespace util {
template<typename S>
struct vec2 {
    using scalar_type = S;

    S x, y;
};
} // namespace util

template<typename Scalar, typename C>
struct std::formatter<util::vec2<Scalar>, C> {
    std::formatter<Scalar, C> scalar_formatter{};

    template<typename ParseContext>
    constexpr auto parse(ParseContext &ctx) {
        return scalar_formatter.parse(ctx);
    }

    template<typename FormatterContext>
    auto format(const util::vec2<Scalar> &v, FormatterContext &ctx) const {
        std::format_to(ctx.out(), "[");
        scalar_formatter.format(v.x, ctx);
        std::format_to(ctx.out(), ", ");
        scalar_formatter.format(v.y, ctx);
        return std::format_to(ctx.out(), "]");
    }
};


#endif // AOC2025_VEC_H
