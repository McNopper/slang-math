#pragma once

#include "float2.hpp"
#include "float3.hpp"
#include "float4.hpp"
#include "float2x2.hpp"
#include "float3x3.hpp"
#include "float4x4.hpp"

#include <algorithm>
#include <cmath>
#include <concepts>
#include <cstdint>
#include <numbers>

namespace sm {

// ── Concepts ──────────────────────────────────────────────────────────────────
//
// The concrete types mirror Slang/HLSL. These concepts let the free functions be
// generic without aliases or surprising overloads, and they state the *full*
// contract the generic bodies depend on (const + mutable indexing, default
// construction, a positive integral static size, an element typedef):
//
//   vec        — any component vector (float2/3/4 AND uint2/3/4). Element-agnostic,
//                so the type-agnostic ops (dot, min, max, clamp, value_ptr) work for
//                every supported element type, and future scalars (e.g. signed int)
//                inherit them automatically.
//   float_vec  — vec whose value_type is float; carries the transcendental /
//                normalizing ops that only make sense for real numbers.
//   square_mat — the row-major float square matrices (float2x2/3x3/4x4). Matrices
//                are float-only by design; an integer-matrix set is not planned.

template<typename V>
concept vec = requires(V v, const V cv, std::int32_t i) {
    typename V::value_type;
    { v[i] }  -> std::same_as<typename V::value_type&>;
    { cv[i] } -> std::same_as<const typename V::value_type&>;
} && std::default_initializable<V>
  && std::integral<decltype(V::size)>
  && requires { V::size > 0; };

template<typename V>
concept float_vec = vec<V> && std::same_as<typename V::value_type, float>;

template<typename M>
concept square_mat = requires(M m, const M cm, std::int32_t i, std::int32_t j) {
    { m[i][j] }  -> std::same_as<float&>;
    { cm[i][j] } -> std::same_as<const float&>;
} && std::default_initializable<M>
  && std::integral<decltype(M::size)>
  && requires { M::size > 0; };

// ── Constants ────────────────────────────────────────────────────────────────

/// pi<float>() / pi<double>() — the mathematical constant pi.
template<typename T> [[nodiscard]] constexpr T pi() noexcept;
template<> [[nodiscard]] inline constexpr float  pi<float>()  noexcept { return std::numbers::pi_v<float>; }
template<> [[nodiscard]] inline constexpr double pi<double>() noexcept { return std::numbers::pi_v<double>; }

// ── Angle conversion ──────────────────────────────────────────────────────────

[[nodiscard]] inline float radians(float deg) noexcept { return deg * (pi<float>() / 180.f); }
[[nodiscard]] inline float degrees(float rad) noexcept { return rad * (180.f / pi<float>()); }

// ── Dot product (element-generic) ─────────────────────────────────────────────

template<vec V>
[[nodiscard]] inline constexpr typename V::value_type dot(const V& a, const V& b) noexcept {
    typename V::value_type s{};
    for (std::int32_t i = 0; i < V::size; ++i) s += a[i] * b[i];
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

template<float_vec V>
[[nodiscard]] inline float length(const V& v) noexcept { return std::sqrt(dot(v, v)); }

template<float_vec V>
[[nodiscard]] inline V normalize(const V& v) noexcept {
    const float len = length(v);
    V r{};
    if (len > 0.f)
        for (std::int32_t i = 0; i < V::size; ++i) r[i] = v[i] / len;
    return r;
}

// ── Component-wise min / max (element-generic) ────────────────────────────────

template<vec V>
[[nodiscard]] inline constexpr V min(const V& a, const V& b) noexcept {
    V r{};
    for (std::int32_t i = 0; i < V::size; ++i) r[i] = std::min(a[i], b[i]);
    return r;
}

template<vec V>
[[nodiscard]] inline constexpr V max(const V& a, const V& b) noexcept {
    V r{};
    for (std::int32_t i = 0; i < V::size; ++i) r[i] = std::max(a[i], b[i]);
    return r;
}

// ── Clamp ─────────────────────────────────────────────────────────────────────

[[nodiscard]] inline constexpr float clamp(float v, float lo, float hi) noexcept {
    return std::clamp(v, lo, hi);
}
template<vec V>
[[nodiscard]] inline constexpr V clamp(const V& v,
                                       typename V::value_type lo,
                                       typename V::value_type hi) noexcept {
    V r{};
    for (std::int32_t i = 0; i < V::size; ++i) r[i] = std::clamp(v[i], lo, hi);
    return r;
}
template<vec V>
[[nodiscard]] inline constexpr V clamp(const V& v, const V& lo, const V& hi) noexcept {
    V r{};
    for (std::int32_t i = 0; i < V::size; ++i) r[i] = std::clamp(v[i], lo[i], hi[i]);
    return r;
}

// ── Lerp (linear interpolation) — canonical Slang/HLSL name ─────────────────────

[[nodiscard]] inline constexpr float  lerp(float  a, float  b, float t) noexcept { return a + t * (b - a); }
template<float_vec V>
[[nodiscard]] inline constexpr V lerp(const V& a, const V& b, float t) noexcept {
    V r{};
    for (std::int32_t i = 0; i < V::size; ++i) r[i] = a[i] + t * (b[i] - a[i]);
    return r;
}
/// Component-wise lerp with a per-component weight.
template<float_vec V>
[[nodiscard]] inline constexpr V lerp(const V& a, const V& b, const V& t) noexcept {
    V r{};
    for (std::int32_t i = 0; i < V::size; ++i) r[i] = a[i] + t[i] * (b[i] - a[i]);
    return r;
}

// ── Abs (real-valued only) ────────────────────────────────────────────────────

template<float_vec V>
[[nodiscard]] inline V abs(const V& v) noexcept {
    V r{};
    for (std::int32_t i = 0; i < V::size; ++i) r[i] = std::abs(v[i]);
    return r;
}

// ── Component-wise elementary functions (real-valued only) ─────────────────────

template<float_vec V> [[nodiscard]] inline V sqrt(const V& v) noexcept {
    V r{}; for (std::int32_t i = 0; i < V::size; ++i) r[i] = std::sqrt(v[i]); return r;
}
template<float_vec V> [[nodiscard]] inline V exp(const V& v) noexcept {
    V r{}; for (std::int32_t i = 0; i < V::size; ++i) r[i] = std::exp(v[i]); return r;
}
template<float_vec V> [[nodiscard]] inline V cos(const V& v) noexcept {
    V r{}; for (std::int32_t i = 0; i < V::size; ++i) r[i] = std::cos(v[i]); return r;
}
template<float_vec V> [[nodiscard]] inline V log(const V& v) noexcept {
    V r{}; for (std::int32_t i = 0; i < V::size; ++i) r[i] = std::log(v[i]); return r;
}
template<float_vec V> [[nodiscard]] inline V pow(const V& v, float p) noexcept {
    V r{}; for (std::int32_t i = 0; i < V::size; ++i) r[i] = std::pow(v[i], p); return r;
}

// ── Smooth-step (scalar) ───────────────────────────────────────────────────────

/// Hermite smooth-step: maps [edge0, edge1] to [0, 1] with zero derivatives at edges.
[[nodiscard]] inline float smoothstep(float edge0, float edge1, float x) noexcept {
    const float t = clamp((x - edge0) / (edge1 - edge0), 0.f, 1.f);
    return t * t * (3.f - 2.f * t);
}

// ── Distance / Reflect (real-valued only) ─────────────────────────────────────

template<float_vec V>
[[nodiscard]] inline float distance(const V& a, const V& b) noexcept {
    return length(b - a);
}

/// Reflect incident vector `i` about unit normal `n`:  i - 2*dot(i,n)*n
template<float_vec V>
[[nodiscard]] inline constexpr V reflect(const V& i, const V& n) noexcept {
    return i - 2.f * dot(i, n) * n;
}

// ── Matrix operations ─────────────────────────────────────────────────────────

/// Transpose — generic over all row-major square matrices.
template<square_mat M>
[[nodiscard]] inline constexpr M transpose(const M& m) noexcept {
    M r{};
    for (std::int32_t i = 0; i < M::size; ++i)
        for (std::int32_t j = 0; j < M::size; ++j)
            r[i][j] = m[j][i];
    return r;
}

/// Matrix × matrix multiply — generic over all row-major square matrices.
/// `result[i][j] = Σ_k a[i][k] * b[k][j]`.
template<square_mat M>
[[nodiscard]] inline constexpr M operator*(const M& a, const M& b) noexcept {
    M r{};
    for (std::int32_t i = 0; i < M::size; ++i)
        for (std::int32_t j = 0; j < M::size; ++j) {
            float s = 0.f;
            for (std::int32_t k = 0; k < M::size; ++k) s += a[i][k] * b[k][j];
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
    return m[0][0]*m[1][1] - m[0][1]*m[1][0];
}

/// 2×2 inverse via closed form.  Returns identity on a singular matrix.
[[nodiscard]] inline float2x2 inverse(const float2x2& m) noexcept {
    const float det = determinant(m);
    if (std::abs(det) < 1e-10f) return float2x2::identity();
    const float inv = 1.f / det;
    // [[a b],[c d]]⁻¹ = (1/det) [[d -b],[-c a]]
    return {
        {  inv * m[1][1], -inv * m[0][1] },
        { -inv * m[1][0],  inv * m[0][0] },
    };
}

/// 3×3 matrix determinant.
[[nodiscard]] inline constexpr float determinant(const float3x3& m) noexcept {
    const float a = m[0][0], b = m[0][1], c = m[0][2];
    const float d = m[1][0], e = m[1][1], f = m[1][2];
    const float g = m[2][0], h = m[2][1], k = m[2][2];
    return a*(e*k - f*h) - b*(d*k - f*g) + c*(d*h - e*g);
}

/// 3×3 matrix inverse via Cramer's rule.  Returns identity on a singular matrix.
[[nodiscard]] inline float3x3 inverse(const float3x3& m) noexcept {
    const float a = m[0][0], b = m[0][1], c = m[0][2];
    const float d = m[1][0], e = m[1][1], f = m[1][2];
    const float g = m[2][0], h = m[2][1], k = m[2][2];

    const float det = a*(e*k - f*h) - b*(d*k - f*g) + c*(d*h - e*g);
    if (std::abs(det) < 1e-10f) return float3x3::identity();
    const float inv = 1.f / det;

    return {
        {  inv*(e*k-f*h), -inv*(b*k-c*h),  inv*(b*f-c*e) },
        { -inv*(d*k-f*g),  inv*(a*k-c*g), -inv*(a*f-c*d) },
        {  inv*(d*h-e*g), -inv*(a*h-b*g),  inv*(a*e-b*d) },
    };
}

/// 4×4 matrix inverse via Gauss-Jordan elimination with partial pivoting.
/// Returns identity on a singular matrix.
[[nodiscard]] inline float4x4 inverse(const float4x4& m) noexcept {
    float aug[4][8]{};
    for (std::int32_t i = 0; i < 4; ++i) {
        for (std::int32_t j = 0; j < 4; ++j) {
            aug[i][j]   = m[i][j];
            aug[i][j+4] = (i == j) ? 1.f : 0.f;
        }
    }

    for (std::int32_t col = 0; col < 4; ++col) {
        std::int32_t pivot = col;
        for (std::int32_t row = col + 1; row < 4; ++row) {
            if (std::abs(aug[row][col]) > std::abs(aug[pivot][col]))
                pivot = row;
        }
        if (pivot != col) {
            for (std::int32_t k = 0; k < 8; ++k)
                std::swap(aug[col][k], aug[pivot][k]);
        }
        const float diag = aug[col][col];
        if (std::abs(diag) < 1e-10f)
            return float4x4::identity();
        const float invDiag = 1.f / diag;
        for (std::int32_t k = 0; k < 8; ++k) aug[col][k] *= invDiag;

        for (std::int32_t row = 0; row < 4; ++row) {
            if (row == col) continue;
            const float f = aug[row][col];
            for (std::int32_t k = 0; k < 8; ++k) aug[row][k] -= f * aug[col][k];
        }
    }

    float4x4 result;
    for (std::int32_t i = 0; i < 4; ++i)
        for (std::int32_t j = 0; j < 4; ++j)
            result[i][j] = aug[i][j + 4];
    return result;
}

/// Normal matrix = transpose(inverse(M)) — generic over all square matrices.
template<square_mat M>
[[nodiscard]] inline M inverseTranspose(const M& m) noexcept {
    return transpose(inverse(m));
}

// ── Sub-matrix extraction (toFloatNxN) ────────────────────────────────────────

/// Extract the upper-left 2×2 submatrix from a 3×3 matrix.
[[nodiscard]] inline constexpr float2x2 toFloat2x2(const float3x3& m) noexcept {
    return { {m[0][0], m[0][1]}, {m[1][0], m[1][1]} };
}

/// Extract the upper-left 2×2 submatrix from a 4×4 matrix.
[[nodiscard]] inline constexpr float2x2 toFloat2x2(const float4x4& m) noexcept {
    return { {m[0][0], m[0][1]}, {m[1][0], m[1][1]} };
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

template<vec V>
[[nodiscard]] inline const typename V::value_type* value_ptr(const V& v) noexcept { return &v[0]; }
template<vec V>
[[nodiscard]] inline typename V::value_type*       value_ptr(V& v)       noexcept { return &v[0]; }
template<square_mat M>
[[nodiscard]] inline const float* value_ptr(const M& m) noexcept { return &m[0][0]; }
template<square_mat M>
[[nodiscard]] inline float*       value_ptr(M& m)       noexcept { return &m[0][0]; }

} // namespace sm
