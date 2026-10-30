#ifndef SLANG_MATH_FUNCTIONS_HPP
#define SLANG_MATH_FUNCTIONS_HPP

#include <algorithm>
#include <cmath>
#include <concepts>
#include <cstdint>
#include <numbers>

#include "float2.hpp"
#include "float2x2.hpp"
#include "float3.hpp"
#include "float3x3.hpp"
#include "float4.hpp"
#include "float4x4.hpp"
#include "operators.hpp"

namespace sm {

constexpr float kSingularEpsilon = 1e-10f;

// ── Concepts ──────────────────────────────────────────────────────────────────
//
// `vec` and `float_vec` (any / float-only component vectors) live in operators.hpp
// so every floatN / uintN header can use the generic arithmetic operators without
// pulling in this file. The square_mat concept is local to the matrix free
// functions below.
//
//   square_mat — the row-major float square matrices (float2x2/3x3/4x4). Matrices
//                are float-only by design; an integer-matrix set is not planned.

template <typename M>
concept square_mat = requires(M m, const M cm, std::int32_t i, std::int32_t j) {
    { m[i][j] } -> std::same_as<float&>;
    { cm[i][j] } -> std::same_as<const float&>;
} && std::default_initializable<M> && std::integral<decltype(M::size)> && requires { M::size > 0; };

// ── Constants ────────────────────────────────────────────────────────────────

/// pi<float>() — the mathematical constant pi.
template <typename T> [[nodiscard]] constexpr T pi() noexcept;
template <> [[nodiscard]] inline constexpr float pi<float>() noexcept {
    return std::numbers::pi_v<float>;
}

// ── Angle conversion ──────────────────────────────────────────────────────────

[[nodiscard]] inline float radians(float deg) noexcept {
    return deg * (pi<float>() / 180.f);
}
[[nodiscard]] inline float degrees(float rad) noexcept {
    return rad * (180.f / pi<float>());
}

// ── Dot product (element-generic) ─────────────────────────────────────────────

template <vec V> [[nodiscard]] inline constexpr typename V::value_type dot(const V& a, const V& b) noexcept {
    typename V::value_type s{};
    for (std::int32_t i = 0; i < V::size; ++i)
        s += a[i] * b[i];
    return s;
}

// ── Cross product (3-D only — cross product is inherently defined on float3) ──

[[nodiscard]] inline constexpr float3 cross(const float3& a, const float3& b) noexcept {
    return {
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x,
    };
}

// ── Length / Normalize (real-valued only) ─────────────────────────────────────

template <float_vec V> [[nodiscard]] inline float length(const V& v) noexcept {
    return std::sqrt(dot(v, v));
}

template <float_vec V> [[nodiscard]] inline V normalize(const V& v) noexcept {
    const float len = length(v);
    V r{};
    if (len > 0.f)
        for (std::int32_t i = 0; i < V::size; ++i)
            r[i] = v[i] / len;
    return r;
}

// ── Component-wise min / max (element-generic) ────────────────────────────────

template <vec V> [[nodiscard]] inline constexpr V min(const V& a, const V& b) noexcept {
    V r{};
    for (std::int32_t i = 0; i < V::size; ++i)
        r[i] = std::min(a[i], b[i]);
    return r;
}

template <vec V> [[nodiscard]] inline constexpr V max(const V& a, const V& b) noexcept {
    V r{};
    for (std::int32_t i = 0; i < V::size; ++i)
        r[i] = std::max(a[i], b[i]);
    return r;
}

// ── Clamp ─────────────────────────────────────────────────────────────────────

[[nodiscard]] inline constexpr float clamp(float v, float lo, float hi) noexcept {
    return std::clamp(v, lo, hi);
}
template <vec V>
[[nodiscard]] inline constexpr V clamp(const V& v, typename V::value_type lo, typename V::value_type hi) noexcept {
    V r{};
    for (std::int32_t i = 0; i < V::size; ++i)
        r[i] = std::clamp(v[i], lo, hi);
    return r;
}
template <vec V> [[nodiscard]] inline constexpr V clamp(const V& v, const V& lo, const V& hi) noexcept {
    V r{};
    for (std::int32_t i = 0; i < V::size; ++i)
        r[i] = std::clamp(v[i], lo[i], hi[i]);
    return r;
}

// ── Saturate (clamp to [0, 1]) — canonical Slang/HLSL name ────────────────────

[[nodiscard]] inline constexpr float saturate(float v) noexcept {
    return std::clamp(v, 0.0f, 1.0f);
}
template <float_vec V> [[nodiscard]] inline constexpr V saturate(const V& v) noexcept {
    V r{};
    for (std::int32_t i = 0; i < V::size; ++i)
        r[i] = std::clamp(v[i], 0.0f, 1.0f);
    return r;
}

// ── Lerp (linear interpolation) — canonical Slang/HLSL name ─────────────────────

[[nodiscard]] inline constexpr float lerp(float a, float b, float t) noexcept {
    return a + t * (b - a);
}
template <float_vec V> [[nodiscard]] inline constexpr V lerp(const V& a, const V& b, float t) noexcept {
    V r{};
    for (std::int32_t i = 0; i < V::size; ++i)
        r[i] = a[i] + t * (b[i] - a[i]);
    return r;
}
/// Component-wise lerp with a per-component weight.
template <float_vec V> [[nodiscard]] inline constexpr V lerp(const V& a, const V& b, const V& t) noexcept {
    V r{};
    for (std::int32_t i = 0; i < V::size; ++i)
        r[i] = a[i] + t[i] * (b[i] - a[i]);
    return r;
}

// ── Abs (real-valued only) ────────────────────────────────────────────────────

template <float_vec V> [[nodiscard]] inline V abs(const V& v) noexcept {
    V r{};
    for (std::int32_t i = 0; i < V::size; ++i)
        r[i] = std::abs(v[i]);
    return r;
}

// ── Component-wise elementary + trig functions (real-valued only) ──────────────

template <float_vec V> [[nodiscard]] inline V sqrt(const V& v) noexcept {
    V r{};
    for (std::int32_t i = 0; i < V::size; ++i)
        r[i] = std::sqrt(v[i]);
    return r;
}
template <float_vec V> [[nodiscard]] inline V exp(const V& v) noexcept {
    V r{};
    for (std::int32_t i = 0; i < V::size; ++i)
        r[i] = std::exp(v[i]);
    return r;
}
template <float_vec V> [[nodiscard]] inline V cos(const V& v) noexcept {
    V r{};
    for (std::int32_t i = 0; i < V::size; ++i)
        r[i] = std::cos(v[i]);
    return r;
}
template <float_vec V> [[nodiscard]] inline V log(const V& v) noexcept {
    V r{};
    for (std::int32_t i = 0; i < V::size; ++i)
        r[i] = std::log(v[i]);
    return r;
}
template <float_vec V> [[nodiscard]] inline V pow(const V& v, float p) noexcept {
    V r{};
    for (std::int32_t i = 0; i < V::size; ++i)
        r[i] = std::pow(v[i], p);
    return r;
}

/// Sine, component-wise. (Vector-only — scalar callers: std::sin.)
template <float_vec V> [[nodiscard]] inline V sin(const V& v) noexcept {
    V r{};
    for (std::int32_t i = 0; i < V::size; ++i)
        r[i] = std::sin(v[i]);
    return r;
}

/// Tangent, component-wise. (Vector-only — scalar callers: std::tan.)
template <float_vec V> [[nodiscard]] inline V tan(const V& v) noexcept {
    V r{};
    for (std::int32_t i = 0; i < V::size; ++i)
        r[i] = std::tan(v[i]);
    return r;
}

/// Arc sine to [-pi/2, pi/2] (|x| <= 1), component-wise. (Vector-only — scalar callers: std::asin.)
template <float_vec V> [[nodiscard]] inline V asin(const V& v) noexcept {
    V r{};
    for (std::int32_t i = 0; i < V::size; ++i)
        r[i] = std::asin(v[i]);
    return r;
}

/// Arc cosine to [0, pi] (|x| <= 1), component-wise. (Vector-only — scalar callers: std::acos.)
template <float_vec V> [[nodiscard]] inline V acos(const V& v) noexcept {
    V r{};
    for (std::int32_t i = 0; i < V::size; ++i)
        r[i] = std::acos(v[i]);
    return r;
}

/// Arc tangent to [-pi/2, pi/2], component-wise. (Vector-only — scalar callers: std::atan.)
template <float_vec V> [[nodiscard]] inline V atan(const V& v) noexcept {
    V r{};
    for (std::int32_t i = 0; i < V::size; ++i)
        r[i] = std::atan(v[i]);
    return r;
}

/// Arc tangent of b/a, quadrant from the signs of both, component-wise. (Vector-only — scalar callers: std::atan2.)
template <float_vec V> [[nodiscard]] inline V atan2(const V& b, const V& a) noexcept {
    V r{};
    for (std::int32_t i = 0; i < V::size; ++i)
        r[i] = std::atan2(b[i], a[i]);
    return r;
}

// ── Common functions (SM1) ──

// Scalar + vector pairs ship for collision-free names (sign, frac, rsqrt,
// step). Names colliding with <cmath>'s global float overloads (floor, ceil,
// trunc, round, fmod, fma, exp2, log2, log10 — like sqrt, exp, cos, log above)
// are VECTOR-ONLY: an unqualified scalar call would be ambiguous; scalar
// callers use std::. The C-runtime-owned names isnan/isinf/isfinite are
// unshippable even for vectors (the UCRT declares unconstrained global
// templates), so the classification helpers carry contract names instead:
// any_nan / any_inf / all_finite.

/// Sign: -1 if x < 0, +1 if x > 0, 0 if x == 0 (NaN yields 0).
[[nodiscard]] inline float sign(float x) noexcept {
    return (x > 0.f) - (x < 0.f);
}
template <float_vec V> [[nodiscard]] inline V sign(const V& v) noexcept {
    V r{};
    for (std::int32_t i = 0; i < V::size; ++i)
        r[i] = (v[i] > 0.f) - (v[i] < 0.f);
    return r;
}

/// Fractional part: x - floor(x), in [0, 1) for finite x.
[[nodiscard]] inline float frac(float x) noexcept {
    return x - std::floor(x);
}
template <float_vec V> [[nodiscard]] inline V frac(const V& v) noexcept {
    V r{};
    for (std::int32_t i = 0; i < V::size; ++i)
        r[i] = v[i] - std::floor(v[i]);
    return r;
}

/// Reciprocal square root: 1 / sqrt(x).  x > 0 for a finite result.
[[nodiscard]] inline float rsqrt(float x) noexcept {
    return 1.f / std::sqrt(x);
}
template <float_vec V> [[nodiscard]] inline V rsqrt(const V& v) noexcept {
    V r{};
    for (std::int32_t i = 0; i < V::size; ++i)
        r[i] = 1.f / std::sqrt(v[i]);
    return r;
}

/// Step: 0 if x < edge, else 1.
[[nodiscard]] inline float step(float edge, float x) noexcept {
    return x < edge ? 0.f : 1.f;
}
template <float_vec V> [[nodiscard]] inline V step(const V& edge, const V& x) noexcept {
    V r{};
    for (std::int32_t i = 0; i < V::size; ++i)
        r[i] = x[i] < edge[i] ? 0.f : 1.f;
    return r;
}

/// Floor, component-wise. (Vector-only — scalar callers: std::floor.)
template <float_vec V> [[nodiscard]] inline V floor(const V& v) noexcept {
    V r{};
    for (std::int32_t i = 0; i < V::size; ++i)
        r[i] = std::floor(v[i]);
    return r;
}

/// Ceiling, component-wise. (Vector-only — scalar callers: std::ceil.)
template <float_vec V> [[nodiscard]] inline V ceil(const V& v) noexcept {
    V r{};
    for (std::int32_t i = 0; i < V::size; ++i)
        r[i] = std::ceil(v[i]);
    return r;
}

/// Truncate (round toward zero), component-wise.
/// (Vector-only — scalar callers: std::trunc.)
template <float_vec V> [[nodiscard]] inline V trunc(const V& v) noexcept {
    V r{};
    for (std::int32_t i = 0; i < V::size; ++i)
        r[i] = std::trunc(v[i]);
    return r;
}

/// Round to the nearest integer, half away from zero (matching std::round),
/// component-wise. (Vector-only — scalar callers: std::round.)
template <float_vec V> [[nodiscard]] inline V round(const V& v) noexcept {
    V r{};
    for (std::int32_t i = 0; i < V::size; ++i)
        r[i] = std::round(v[i]);
    return r;
}

/// Floating-point modulo x - y * trunc(x / y); the sign follows x,
/// component-wise. (Vector-only — scalar callers: std::fmod.)
template <float_vec V> [[nodiscard]] inline V fmod(const V& x, const V& y) noexcept {
    V r{};
    for (std::int32_t i = 0; i < V::size; ++i)
        r[i] = std::fmod(x[i], y[i]);
    return r;
}

/// Fused multiply-add a * b + c, component-wise.
/// (Vector-only — scalar callers: std::fma.)
template <float_vec V> [[nodiscard]] inline V fma(const V& a, const V& b, const V& c) noexcept {
    V r{};
    for (std::int32_t i = 0; i < V::size; ++i)
        r[i] = std::fma(a[i], b[i], c[i]);
    return r;
}

/// Base-2 exponential, component-wise.
/// (Vector-only — scalar callers: std::exp2.)
template <float_vec V> [[nodiscard]] inline V exp2(const V& v) noexcept {
    V r{};
    for (std::int32_t i = 0; i < V::size; ++i)
        r[i] = std::exp2(v[i]);
    return r;
}

/// Base-2 logarithm (x > 0 for a finite result), component-wise.
/// (Vector-only — scalar callers: std::log2.)
template <float_vec V> [[nodiscard]] inline V log2(const V& v) noexcept {
    V r{};
    for (std::int32_t i = 0; i < V::size; ++i)
        r[i] = std::log2(v[i]);
    return r;
}

/// Base-10 logarithm (x > 0 for a finite result), component-wise.
/// (Vector-only — scalar callers: std::log10.)
template <float_vec V> [[nodiscard]] inline V log10(const V& v) noexcept {
    V r{};
    for (std::int32_t i = 0; i < V::size; ++i)
        r[i] = std::log10(v[i]);
    return r;
}

/// Per-component power: pow(base, exponent) component-wise.
template <float_vec V> [[nodiscard]] inline V pow(const V& base, const V& exponent) noexcept {
    V r{};
    for (std::int32_t i = 0; i < V::size; ++i)
        r[i] = std::pow(base[i], exponent[i]);
    return r;
}

/// True when ANY component is NaN. (Contract-named: HLSL/Slang isnan is
/// unshippable — the UCRT declares global template isnan/isinf/isfinite
/// overloads that collide even for vector arguments.)
template <float_vec V> [[nodiscard]] inline bool any_nan(const V& v) noexcept {
    for (std::int32_t i = 0; i < V::size; ++i)
        if (std::isnan(v[i]))
            return true;
    return false;
}

/// True when ANY component is ±infinity.
template <float_vec V> [[nodiscard]] inline bool any_inf(const V& v) noexcept {
    for (std::int32_t i = 0; i < V::size; ++i)
        if (std::isinf(v[i]))
            return true;
    return false;
}

/// True when ALL components are finite (neither NaN nor ±infinity).
template <float_vec V> [[nodiscard]] inline bool all_finite(const V& v) noexcept {
    for (std::int32_t i = 0; i < V::size; ++i)
        if (!std::isfinite(v[i]))
            return false;
    return true;
}

// ── Smooth-step (scalar + vector) ──────────────────────────────────────────────

/// Hermite smooth-step: maps [edge0, edge1] to [0, 1] with zero derivatives at edges.
[[nodiscard]] inline float smoothstep(float edge0, float edge1, float x) noexcept {
    const float t = clamp((x - edge0) / (edge1 - edge0), 0.f, 1.f);
    return t * t * (3.f - 2.f * t);
}

/// Hermite smooth-step, component-wise: maps [edge0, edge1] to [0, 1] per component.
template <float_vec V> [[nodiscard]] inline V smoothstep(const V& edge0, const V& edge1, const V& x) noexcept {
    V r{};
    for (std::int32_t i = 0; i < V::size; ++i) {
        const float t = clamp((x[i] - edge0[i]) / (edge1[i] - edge0[i]), 0.f, 1.f);
        r[i] = t * t * (3.f - 2.f * t);
    }
    return r;
}

// ── Distance / Reflect / Refract (real-valued only) ───────────────────────────

template <float_vec V> [[nodiscard]] inline float distance(const V& a, const V& b) noexcept {
    return length(b - a);
}

/// Reflect incident vector `i` about unit normal `n`:  i - 2*dot(i,n)*n
template <float_vec V> [[nodiscard]] inline constexpr V reflect(const V& i, const V& n) noexcept {
    return i - 2.f * dot(i, n) * n;
}

/// Flip `n` to face incident `i`: dot(nref, i) < 0 ? n : -n (HLSL faceforward semantics).
template <float_vec V> [[nodiscard]] inline V faceforward(const V& n, const V& i, const V& nref) noexcept {
    return dot(nref, i) < 0.f ? n : -n;
}

/// Refract incident vector `i` through unit normal `n` with eta = ni/nt (scalar).
/// Total internal reflection (k = 1 - eta²(1 - dot(n,i)²) < 0) returns the ZERO vector.
template <float_vec V> [[nodiscard]] inline V refract(const V& i, const V& n, float eta) noexcept {
    const float d = dot(n, i);
    const float k = 1.f - eta * eta * (1.f - d * d);
    if (k < 0.f)
        return V{};
    return eta * i - (eta * d + std::sqrt(k)) * n;
}

// ── Matrix operations ─────────────────────────────────────────────────────────

/// Transpose — generic over all row-major square matrices.
template <square_mat M> [[nodiscard]] inline constexpr M transpose(const M& m) noexcept {
    M r{};
    for (std::int32_t i = 0; i < M::size; ++i)
        for (std::int32_t j = 0; j < M::size; ++j)
            r[i][j] = m[j][i];
    return r;
}

/// Matrix × matrix multiply — generic over all row-major square matrices.
/// `result[i][j] = Σ_k a[i][k] * b[k][j]`.
template <square_mat M> [[nodiscard]] inline constexpr M operator*(const M& a, const M& b) noexcept {
    M r{};
    for (std::int32_t i = 0; i < M::size; ++i)
        for (std::int32_t j = 0; j < M::size; ++j) {
            float s = 0.f;
            for (std::int32_t k = 0; k < M::size; ++k)
                s += a[i][k] * b[k][j];
            r[i][j] = s;
        }
    return r;
}

// ── Inverse / determinant — size-specialized ──────────────────────────────────
//
// Each size keeps its optimal algorithm; do NOT collapse into a single generic
// inverse: the 2×2 / 3×3 closed forms are far cheaper than a 4×4 Gauss-Jordan.

/// 2×2 determinant.
[[nodiscard]] inline constexpr float determinant(const float2x2& m) noexcept {
    return m[0][0] * m[1][1] - m[0][1] * m[1][0];
}

/// 2×2 inverse via closed form.  Returns identity on a singular matrix.
[[nodiscard]] inline float2x2 inverse(const float2x2& m) noexcept {
    const float det = determinant(m);
    if (std::abs(det) < kSingularEpsilon)
        return float2x2::identity();
    const float inv = 1.f / det;
    // [[a b],[c d]]⁻¹ = (1/det) [[d -b],[-c a]]
    return {
        {inv * m[1][1], -inv * m[0][1]},
        {-inv * m[1][0], inv * m[0][0]},
    };
}

/// 3×3 matrix determinant.
[[nodiscard]] inline constexpr float determinant(const float3x3& m) noexcept {
    const float a = m[0][0], b = m[0][1], c = m[0][2];
    const float d = m[1][0], e = m[1][1], f = m[1][2];
    const float g = m[2][0], h = m[2][1], k = m[2][2];
    return a * (e * k - f * h) - b * (d * k - f * g) + c * (d * h - e * g);
}

/// 3×3 matrix inverse via Cramer's rule.  Returns identity on a singular matrix.
[[nodiscard]] inline float3x3 inverse(const float3x3& m) noexcept {
    const float a = m[0][0], b = m[0][1], c = m[0][2];
    const float d = m[1][0], e = m[1][1], f = m[1][2];
    const float g = m[2][0], h = m[2][1], k = m[2][2];

    const float det = determinant(m);
    if (std::abs(det) < kSingularEpsilon)
        return float3x3::identity();
    const float inv = 1.f / det;

    return {
        {inv * (e * k - f * h), -inv * (b * k - c * h), inv * (b * f - c * e)},
        {-inv * (d * k - f * g), inv * (a * k - c * g), -inv * (a * f - c * d)},
        {inv * (d * h - e * g), -inv * (a * h - b * g), inv * (a * e - b * d)},
    };
}

/// 4×4 matrix determinant — cofactor expansion along the first row, in terms of
/// the existing 3×3 determinants of the row-0 minors.
[[nodiscard]] inline constexpr float determinant(const float4x4& m) noexcept {
    const float3x3 minor0{{m[1][1], m[1][2], m[1][3]}, {m[2][1], m[2][2], m[2][3]}, {m[3][1], m[3][2], m[3][3]}};
    const float3x3 minor1{{m[1][0], m[1][2], m[1][3]}, {m[2][0], m[2][2], m[2][3]}, {m[3][0], m[3][2], m[3][3]}};
    const float3x3 minor2{{m[1][0], m[1][1], m[1][3]}, {m[2][0], m[2][1], m[2][3]}, {m[3][0], m[3][1], m[3][3]}};
    const float3x3 minor3{{m[1][0], m[1][1], m[1][2]}, {m[2][0], m[2][1], m[2][2]}, {m[3][0], m[3][1], m[3][2]}};
    return m[0][0] * determinant(minor0) - m[0][1] * determinant(minor1) + m[0][2] * determinant(minor2) -
           m[0][3] * determinant(minor3);
}

namespace detail {

inline constexpr std::int32_t matrixCols = 4;                 ///< float4x4 dimension
inline constexpr std::int32_t augmentedCols = matrixCols * 2; ///< matrix columns + identity columns

struct AugmentedMatrix {
    float rows[matrixCols][augmentedCols]{};
};

[[nodiscard]] inline constexpr AugmentedMatrix buildAugmented(const float4x4& m) noexcept {
    AugmentedMatrix aug{};
    for (std::int32_t i = 0; i < 4; ++i)
        for (std::int32_t j = 0; j < 4; ++j) {
            aug.rows[i][j] = m[i][j];
            aug.rows[i][j + matrixCols] = (i == j) ? 1.f : 0.f;
        }
    return aug;
}

[[nodiscard]] inline std::int32_t partialPivot(const AugmentedMatrix& aug, std::int32_t col) noexcept {
    std::int32_t pivot = col;
    for (std::int32_t row = col + 1; row < 4; ++row)
        if (std::abs(aug.rows[row][col]) > std::abs(aug.rows[pivot][col]))
            pivot = row;
    return pivot;
}

inline constexpr void normalizePivotRow(AugmentedMatrix& aug, std::int32_t col) noexcept {
    const float invDiag = 1.f / aug.rows[col][col];
    for (std::int32_t k = 0; k < augmentedCols; ++k)
        aug.rows[col][k] *= invDiag;
}

inline constexpr void eliminateRows(AugmentedMatrix& aug, std::int32_t col) noexcept {
    for (std::int32_t row = 0; row < 4; ++row) {
        if (row == col)
            continue;
        const float f = aug.rows[row][col];
        for (std::int32_t k = 0; k < augmentedCols; ++k)
            aug.rows[row][k] -= f * aug.rows[col][k];
    }
}

[[nodiscard]] inline constexpr float4x4 extractRightHalf(const AugmentedMatrix& aug) noexcept {
    float4x4 result;
    for (std::int32_t i = 0; i < 4; ++i)
        for (std::int32_t j = 0; j < 4; ++j)
            result[i][j] = aug.rows[i][j + matrixCols];
    return result;
}

} // namespace detail

/// 4×4 matrix inverse via Gauss-Jordan elimination with partial pivoting.
/// Returns identity on a singular matrix.
[[nodiscard]] inline float4x4 inverse(const float4x4& m) noexcept {
    detail::AugmentedMatrix aug = detail::buildAugmented(m);

    for (std::int32_t col = 0; col < 4; ++col) {
        const std::int32_t pivot = detail::partialPivot(aug, col);
        if (pivot != col)
            for (std::int32_t k = 0; k < detail::augmentedCols; ++k)
                std::swap(aug.rows[col][k], aug.rows[pivot][k]);
        if (std::abs(aug.rows[col][col]) < kSingularEpsilon)
            return float4x4::identity();
        detail::normalizePivotRow(aug, col);
        detail::eliminateRows(aug, col);
    }

    return detail::extractRightHalf(aug);
}

/// Normal matrix = transpose(inverse(M)) — generic over all square matrices.
template <square_mat M> [[nodiscard]] inline M inverseTranspose(const M& m) noexcept {
    return transpose(inverse(m));
}

// ── Sub-matrix extraction (toFloatNxN) ────────────────────────────────────────

/// Extract the upper-left 2×2 submatrix from a 3×3 matrix.
[[nodiscard]] inline constexpr float2x2 toFloat2x2(const float3x3& m) noexcept {
    return {{m[0][0], m[0][1]}, {m[1][0], m[1][1]}};
}

/// Extract the upper-left 2×2 submatrix from a 4×4 matrix.
[[nodiscard]] inline constexpr float2x2 toFloat2x2(const float4x4& m) noexcept {
    return {{m[0][0], m[0][1]}, {m[1][0], m[1][1]}};
}

/// Extract the upper-left 3×3 submatrix from a 4×4 matrix.
/// Equivalent to float3x3(m) truncation in Slang shaders.
[[nodiscard]] inline constexpr float3x3 toFloat3x3(const float4x4& m) noexcept {
    return {
        {m[0][0], m[0][1], m[0][2]},
        {m[1][0], m[1][1], m[1][2]},
        {m[2][0], m[2][1], m[2][2]},
    };
}

// ── Pointer access (Vulkan buffer uploads) ────────────────────────────────────
//
// value_ptr returns a pointer to the first element — `&v[0]` / `&m[0][0]`.
// On vectors the pointee is the vector's value_type; on matrices it is float.

template <vec V> [[nodiscard]] inline const typename V::value_type* value_ptr(const V& v) noexcept {
    return &v[0];
}
template <vec V> [[nodiscard]] inline typename V::value_type* value_ptr(V& v) noexcept {
    return &v[0];
}
template <square_mat M> [[nodiscard]] inline const float* value_ptr(const M& m) noexcept {
    return &m[0][0];
}
template <square_mat M> [[nodiscard]] inline float* value_ptr(M& m) noexcept {
    return &m[0][0];
}

} // namespace sm
#endif // SLANG_MATH_FUNCTIONS_HPP
