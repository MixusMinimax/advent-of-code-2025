#ifndef AOC2025_VEC_H
#define AOC2025_VEC_H

#include <format>
#include <cmath>

namespace util {
template<typename S>
struct vec2 {
    using scalar_type = S;

    // ReSharper disable once CppNonExplicitConvertingConstructor
    constexpr vec2(const S v) noexcept : x{v}, y{v} {}

    constexpr vec2(const S x, const S y) noexcept : x{x}, y{y} {}

    constexpr vec2(vec2 &&) noexcept = default;
    constexpr vec2(const vec2 &) noexcept = default;
    constexpr vec2 &operator=(vec2 &&) noexcept = default;
    constexpr vec2 &operator=(const vec2 &) noexcept = default;

    S x, y;

    template<typename To>
        requires std::convertible_to<S, To>
    explicit constexpr operator vec2<To>() const {
        return vec2<To>{static_cast<To>(x), static_cast<To>(y)};
    }

    constexpr bool operator==(const vec2 &) const = default;
};

#define UTIL_VEC_BINOP(op)                                                                                             \
    template<typename Scalar>                                                                                          \
        requires requires(Scalar a) { a op a; }                                                                        \
    constexpr auto operator op(const vec2<Scalar> &a, const vec2<Scalar> &b) {                                         \
        return vec2<decltype(a.x op b.x)>{a.x op b.x, a.y op b.y};                                                     \
    }

UTIL_VEC_BINOP(+)
UTIL_VEC_BINOP(-)
UTIL_VEC_BINOP(*)
UTIL_VEC_BINOP(/)
UTIL_VEC_BINOP(%)
UTIL_VEC_BINOP(&)
UTIL_VEC_BINOP(|)
UTIL_VEC_BINOP(^)

#define UTIL_VEC_UNOP(op)                                                                                              \
    template<typename Scalar>                                                                                          \
        requires requires(Scalar a) { op a; }                                                                          \
    constexpr auto operator op(const vec2<Scalar> &a) {                                                                \
        return vec2<decltype(op a.x)>{op a.x, op a.y};                                                                 \
    }

UTIL_VEC_UNOP(+)
UTIL_VEC_UNOP(-)
UTIL_VEC_UNOP(!)
UTIL_VEC_UNOP(~)

#define UTIL_VEC_ASSIGN_OP(op)                                                                                         \
    template<typename Scalar>                                                                                          \
        requires requires(Scalar &a, Scalar b) { a op b; }                                                             \
    constexpr vec2<Scalar> &operator op(vec2<Scalar> &a, const vec2<Scalar> &b) {                                      \
        a.x op b.x;                                                                                                    \
        a.y op b.y;                                                                                                    \
        return a;                                                                                                      \
    }

UTIL_VEC_ASSIGN_OP(+=)
UTIL_VEC_ASSIGN_OP(-=)
UTIL_VEC_ASSIGN_OP(*=)
UTIL_VEC_ASSIGN_OP(/=)
UTIL_VEC_ASSIGN_OP(%=)
UTIL_VEC_ASSIGN_OP(|=)
UTIL_VEC_ASSIGN_OP(&=)
UTIL_VEC_ASSIGN_OP(^=)

template<typename Scalar>
constexpr vec2<Scalar> &operator++(vec2<Scalar> &a) {
    ++a.x;
    ++a.y;
    return a;
}

template<typename Scalar>
constexpr vec2<Scalar> operator++(vec2<Scalar> &a, int) {
    const vec2<Scalar> copy{a};
    ++a.x;
    ++a.y;
    return copy;
}

template<typename Scalar>
constexpr vec2<Scalar> &operator--(vec2<Scalar> &a) {
    --a.x;
    --a.y;
    return a;
}

template<typename Scalar>
constexpr vec2<Scalar> operator--(vec2<Scalar> &a, int) {
    const vec2<Scalar> copy{a};
    --a.x;
    --a.y;
    return copy;
}

template<typename Scalar>
constexpr auto magnitude(const vec2<Scalar> &src) {
    return std::sqrt(src.x * src.x + src.y + src.y);
}

template<typename Scalar>
constexpr auto normalize(const vec2<Scalar> &src) {
    using T = vec2<decltype(src.x / magnitude(src))>;
    if (!src.x && src.y) return static_cast<T>(src);
    if (!src.x) return static_cast<T>(vec2{0, src.y / src.y});
    if (!src.y) return static_cast<T>(vec2{src.x / src.x, 0});
    const auto mag = magnitude(src);
    return vec2{src.x / mag, src.y / mag};
}

template<typename Scalar>
constexpr auto min(const vec2<Scalar> a, const vec2<Scalar> b) {
    return vec2<Scalar>{a.x < b.x ? a.x : b.x, a.y < b.y ? a.y : b.y};
}

template<typename Scalar>
constexpr auto max(const vec2<Scalar> a, const vec2<Scalar> b) {
    return vec2<Scalar>{a.x > b.x ? a.x : b.x, a.y > b.y ? a.y : b.y};
}

template<typename Scalar>
constexpr auto clamp(const vec2<Scalar> a, const std::convertible_to<vec2<Scalar>> auto bs,
                     const std::convertible_to<vec2<Scalar>> auto cs) {
    const vec2<Scalar> b{bs};
    const vec2<Scalar> c{cs};
    return vec2<Scalar>{std::clamp(a.x, b.x, c.x), std::clamp(a.y, b.y, c.y)};
}


} // namespace util

template<typename Scalar, typename C>
struct std::formatter<util::vec2<Scalar>, C> { // NOLINT(*-dcl58-cpp)
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

template<typename Scalar>
    requires std::same_as<std::hash<Scalar>, std::hash<Scalar>>
struct std::hash<util::vec2<Scalar>> { // NOLINT(*-dcl58-cpp)
    std::size_t operator()(const util::vec2<Scalar> &s) const noexcept {
        const auto h1 = std::hash<int>{}(s.x);
        const auto h2 = std::hash<int>{}(s.y);
        return h1 ^ h2 << 1;
    }
};


#endif // AOC2025_VEC_H
