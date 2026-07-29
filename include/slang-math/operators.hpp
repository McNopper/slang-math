#ifndef SLANG_MATH_OPERATORS_HPP
#define SLANG_MATH_OPERATORS_HPP

#include <concepts>
#include <cstdint>

namespace sm {

// ── Concepts ──────────────────────────────────────────────────────────────────
//
// The concrete types mirror Slang/HLSL. These concepts let the arithmetic
// operators (and the free functions in functions.hpp) be generic without aliases
// or surprising overloads, and they state the *full* contract the generic bodies
// depend on (const + mutable indexing, default construction, a positive integral
// static size, an element typedef):
//
//   vec        — any component vector (float2/3/4 AND uint2/3/4). Element-agnostic,
//                so the type-agnostic ops (+, -, *, min, max, clamp, value_ptr)
//                work for every supported element type, and future scalars (e.g.
//                signed int) inherit them automatically.
//   float_vec  — vec whose value_type is float; carries the division ops and the
//                transcendental / normalizing ops that only make sense for real
//                numbers. Integer vectors deliberately omit division.
//
// This header is intentionally lightweight (no type headers) so every floatN /
// uintN header can include it without forming an include cycle with
// functions.hpp. The operator templates are only instantiated at the call site,
// by which point the concrete vector type is complete.

template <typename V>
concept vec = requires(V v, const V cv, std::int32_t i) {
    typename V::value_type;
    { v[i] } -> std::same_as<typename V::value_type&>;
    { cv[i] } -> std::same_as<const typename V::value_type&>;
} && std::default_initializable<V> && std::integral<decltype(V::size)> && requires { V::size > 0; };

template <typename V>
concept float_vec = vec<V> && std::same_as<typename V::value_type, float>;

// ── Generic vector arithmetic operators ───────────────────────────────────────
//
// Replaces the per-arity member operators that float2/3/4 and uint2/3/4 used to
// hand-write. The bodies are identical for every size and element type — only
// division is restricted to float_vec, matching the original uintN behaviour
// (which never offered /). Every operator is constexpr to preserve the
// compile-time evaluability of the hand-written members it replaces.

// Component-wise binary: +, -, *  (any vec); /  (float_vec only).

template <vec V> [[nodiscard]] inline constexpr V operator+(const V& a, const V& b) noexcept {
    V r{};
    for (std::int32_t i = 0; i < V::size; ++i)
        r[i] = a[i] + b[i];
    return r;
}
template <vec V> [[nodiscard]] inline constexpr V operator-(const V& a, const V& b) noexcept {
    V r{};
    for (std::int32_t i = 0; i < V::size; ++i)
        r[i] = a[i] - b[i];
    return r;
}
template <vec V> [[nodiscard]] inline constexpr V operator*(const V& a, const V& b) noexcept {
    V r{};
    for (std::int32_t i = 0; i < V::size; ++i)
        r[i] = a[i] * b[i];
    return r;
}
template <float_vec V> [[nodiscard]] inline constexpr V operator/(const V& a, const V& b) noexcept {
    V r{};
    for (std::int32_t i = 0; i < V::size; ++i)
        r[i] = a[i] / b[i];
    return r;
}

// Scalar on the right: v + s, v - s, v * s  (any vec); v / s  (float_vec only).

template <vec V> [[nodiscard]] inline constexpr V operator+(const V& a, typename V::value_type s) noexcept {
    V r{};
    for (std::int32_t i = 0; i < V::size; ++i)
        r[i] = a[i] + s;
    return r;
}
template <vec V> [[nodiscard]] inline constexpr V operator-(const V& a, typename V::value_type s) noexcept {
    V r{};
    for (std::int32_t i = 0; i < V::size; ++i)
        r[i] = a[i] - s;
    return r;
}
template <vec V> [[nodiscard]] inline constexpr V operator*(const V& a, typename V::value_type s) noexcept {
    V r{};
    for (std::int32_t i = 0; i < V::size; ++i)
        r[i] = a[i] * s;
    return r;
}
template <float_vec V> [[nodiscard]] inline constexpr V operator/(const V& a, typename V::value_type s) noexcept {
    V r{};
    for (std::int32_t i = 0; i < V::size; ++i)
        r[i] = a[i] / s;
    return r;
}

// Scalar on the left: s + v, s - v, s * v  (any vec); s / v  (float_vec only).

template <vec V> [[nodiscard]] inline constexpr V operator+(typename V::value_type s, const V& a) noexcept {
    V r{};
    for (std::int32_t i = 0; i < V::size; ++i)
        r[i] = s + a[i];
    return r;
}
template <vec V> [[nodiscard]] inline constexpr V operator-(typename V::value_type s, const V& a) noexcept {
    V r{};
    for (std::int32_t i = 0; i < V::size; ++i)
        r[i] = s - a[i];
    return r;
}
template <vec V> [[nodiscard]] inline constexpr V operator*(typename V::value_type s, const V& a) noexcept {
    V r{};
    for (std::int32_t i = 0; i < V::size; ++i)
        r[i] = s * a[i];
    return r;
}
template <float_vec V> [[nodiscard]] inline constexpr V operator/(typename V::value_type s, const V& a) noexcept {
    V r{};
    for (std::int32_t i = 0; i < V::size; ++i)
        r[i] = s / a[i];
    return r;
}

// Unary negate (any vec).

template <vec V> [[nodiscard]] inline constexpr V operator-(const V& v) noexcept {
    V r{};
    for (std::int32_t i = 0; i < V::size; ++i)
        r[i] = -v[i];
    return r;
}

// Component-wise compound assignment: +=, -=, *=  (any vec); /=  (float_vec only).

template <vec V> inline constexpr V& operator+=(V& a, const V& b) noexcept {
    for (std::int32_t i = 0; i < V::size; ++i)
        a[i] += b[i];
    return a;
}
template <vec V> inline constexpr V& operator-=(V& a, const V& b) noexcept {
    for (std::int32_t i = 0; i < V::size; ++i)
        a[i] -= b[i];
    return a;
}
template <vec V> inline constexpr V& operator*=(V& a, const V& b) noexcept {
    for (std::int32_t i = 0; i < V::size; ++i)
        a[i] *= b[i];
    return a;
}
template <float_vec V> inline constexpr V& operator/=(V& a, const V& b) noexcept {
    for (std::int32_t i = 0; i < V::size; ++i)
        a[i] /= b[i];
    return a;
}

// Scalar compound assignment: +=, -=, *=  (any vec); /=  (float_vec only).

template <vec V> inline constexpr V& operator+=(V& a, typename V::value_type s) noexcept {
    for (std::int32_t i = 0; i < V::size; ++i)
        a[i] += s;
    return a;
}
template <vec V> inline constexpr V& operator-=(V& a, typename V::value_type s) noexcept {
    for (std::int32_t i = 0; i < V::size; ++i)
        a[i] -= s;
    return a;
}
template <vec V> inline constexpr V& operator*=(V& a, typename V::value_type s) noexcept {
    for (std::int32_t i = 0; i < V::size; ++i)
        a[i] *= s;
    return a;
}
template <float_vec V> inline constexpr V& operator/=(V& a, typename V::value_type s) noexcept {
    for (std::int32_t i = 0; i < V::size; ++i)
        a[i] /= s;
    return a;
}

} // namespace sm
#endif // SLANG_MATH_OPERATORS_HPP
