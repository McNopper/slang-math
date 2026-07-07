#pragma once

#include "float2.hpp"
#include "float3.hpp"
#include "float4.hpp"
#include "float3x3.hpp"
#include "float4x4.hpp"

#include <algorithm>
#include <cmath>
#include <numbers>

namespace sm {

// ── Constants ────────────────────────────────────────────────────────────────

/// pi<float>() / pi<double>() — equivalent to glm::pi<T>().
template<typename T> [[nodiscard]] constexpr T pi() noexcept;
template<> [[nodiscard]] inline constexpr float  pi<float>()  noexcept { return std::numbers::pi_v<float>; }
template<> [[nodiscard]] inline constexpr double pi<double>() noexcept { return std::numbers::pi_v<double>; }

// ── Angle conversion ──────────────────────────────────────────────────────────

[[nodiscard]] inline float radians(float deg) noexcept { return deg * (pi<float>() / 180.f); }
[[nodiscard]] inline float degrees(float rad) noexcept { return rad * (180.f / pi<float>()); }

// ── Dot product ───────────────────────────────────────────────────────────────

[[nodiscard]] inline constexpr float dot(const float2& a, const float2& b) noexcept {
    return a.x * b.x + a.y * b.y;
}
[[nodiscard]] inline constexpr float dot(const float3& a, const float3& b) noexcept {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}
[[nodiscard]] inline constexpr float dot(const float4& a, const float4& b) noexcept {
    return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

// ── Cross product ─────────────────────────────────────────────────────────────

[[nodiscard]] inline constexpr float3 cross(const float3& a, const float3& b) noexcept {
    return {
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x,
    };
}

// ── Length ───────────────────────────────────────────────────────────────────

[[nodiscard]] inline float length(const float2& v) noexcept { return std::sqrt(dot(v, v)); }
[[nodiscard]] inline float length(const float3& v) noexcept { return std::sqrt(dot(v, v)); }
[[nodiscard]] inline float length(const float4& v) noexcept { return std::sqrt(dot(v, v)); }

// ── Normalize ─────────────────────────────────────────────────────────────────

[[nodiscard]] inline float2 normalize(const float2& v) noexcept {
    const float len = length(v);
    return len > 0.f ? v / len : float2{};
}
[[nodiscard]] inline float3 normalize(const float3& v) noexcept {
    const float len = length(v);
    return len > 0.f ? v / len : float3{};
}
[[nodiscard]] inline float4 normalize(const float4& v) noexcept {
    const float len = length(v);
    return len > 0.f ? v / len : float4{};
}

// ── Component-wise min / max ──────────────────────────────────────────────────

[[nodiscard]] inline constexpr float2 min(const float2& a, const float2& b) noexcept {
    return {std::min(a.x, b.x), std::min(a.y, b.y)};
}
[[nodiscard]] inline constexpr float3 min(const float3& a, const float3& b) noexcept {
    return {std::min(a.x, b.x), std::min(a.y, b.y), std::min(a.z, b.z)};
}
[[nodiscard]] inline constexpr float4 min(const float4& a, const float4& b) noexcept {
    return {std::min(a.x, b.x), std::min(a.y, b.y), std::min(a.z, b.z), std::min(a.w, b.w)};
}

[[nodiscard]] inline constexpr float2 max(const float2& a, const float2& b) noexcept {
    return {std::max(a.x, b.x), std::max(a.y, b.y)};
}
[[nodiscard]] inline constexpr float3 max(const float3& a, const float3& b) noexcept {
    return {std::max(a.x, b.x), std::max(a.y, b.y), std::max(a.z, b.z)};
}
[[nodiscard]] inline constexpr float4 max(const float4& a, const float4& b) noexcept {
    return {std::max(a.x, b.x), std::max(a.y, b.y), std::max(a.z, b.z), std::max(a.w, b.w)};
}

// ── Clamp ─────────────────────────────────────────────────────────────────────

[[nodiscard]] inline constexpr float clamp(float v, float lo, float hi) noexcept {
    return std::clamp(v, lo, hi);
}
[[nodiscard]] inline constexpr float3 clamp(const float3& v, float lo, float hi) noexcept {
    return {clamp(v.x, lo, hi), clamp(v.y, lo, hi), clamp(v.z, lo, hi)};
}
[[nodiscard]] inline constexpr float3 clamp(const float3& v, const float3& lo, const float3& hi) noexcept {
    return {clamp(v.x, lo.x, hi.x), clamp(v.y, lo.y, hi.y), clamp(v.z, lo.z, hi.z)};
}

// ── Lerp (linear interpolation) — canonical Slang/HLSL name ─────────────────

[[nodiscard]] inline constexpr float  lerp(float  a, float  b, float t) noexcept { return a + t * (b - a); }
[[nodiscard]] inline constexpr float3 lerp(const float3& a, const float3& b, float t) noexcept {
    return a + t * (b - a);
}

/// @deprecated Use lerp(). GLSL compatibility alias — will be removed in a future version.
[[nodiscard]] inline constexpr float  mix(float  a, float  b, float t) noexcept { return lerp(a, b, t); }
/// @deprecated Use lerp(). GLSL compatibility alias — will be removed in a future version.
[[nodiscard]] inline constexpr float3 mix(const float3& a, const float3& b, float t) noexcept { return lerp(a, b, t); }

// ── Abs ───────────────────────────────────────────────────────────────────────

[[nodiscard]] inline float3 abs(const float3& v) noexcept {
    return {std::abs(v.x), std::abs(v.y), std::abs(v.z)};
}
[[nodiscard]] inline float4 abs(const float4& v) noexcept {
    return {std::abs(v.x), std::abs(v.y), std::abs(v.z), std::abs(v.w)};
}

// ── Component-wise elementary functions ──────────────────────────────────────

[[nodiscard]] inline float3 sqrt(const float3& v) noexcept {
    return {std::sqrt(v.x), std::sqrt(v.y), std::sqrt(v.z)};
}
[[nodiscard]] inline float3 exp(const float3& v) noexcept {
    return {std::exp(v.x), std::exp(v.y), std::exp(v.z)};
}
[[nodiscard]] inline float3 cos(const float3& v) noexcept {
    return {std::cos(v.x), std::cos(v.y), std::cos(v.z)};
}
[[nodiscard]] inline float3 pow(const float3& v, float p) noexcept {
    return {std::pow(v.x, p), std::pow(v.y, p), std::pow(v.z, p)};
}
[[nodiscard]] inline float3 log(const float3& v) noexcept {
    return {std::log(v.x), std::log(v.y), std::log(v.z)};
}

// ── Smooth-step ───────────────────────────────────────────────────────────────

/// Hermite smooth-step: maps [edge0, edge1] to [0, 1] with zero derivatives at edges.
/// Equivalent to glm::smoothstep(edge0, edge1, x).
[[nodiscard]] inline float smoothstep(float edge0, float edge1, float x) noexcept {
    const float t = clamp((x - edge0) / (edge1 - edge0), 0.f, 1.f);
    return t * t * (3.f - 2.f * t);
}

// ── Mix with vector weight (lerp with per-component t) ───────────────────────

/// Component-wise lerp: lerp(a, b, t) = a + t * (b - a).
[[nodiscard]] inline constexpr float3 lerp(const float3& a, const float3& b, const float3& t) noexcept {
    return {a.x + t.x*(b.x-a.x), a.y + t.y*(b.y-a.y), a.z + t.z*(b.z-a.z)};
}
/// @deprecated Use lerp(). GLSL compatibility alias — will be removed in a future version.
[[nodiscard]] inline constexpr float3 mix(const float3& a, const float3& b, const float3& t) noexcept { return lerp(a, b, t); }

// ── Distance ─────────────────────────────────────────────────────────────────

[[nodiscard]] inline float distance(const float3& a, const float3& b) noexcept {
    return length(b - a);
}

// ── Reflect ──────────────────────────────────────────────────────────────────

/// Reflect incident vector `i` about unit normal `n`:  i - 2*dot(i,n)*n
[[nodiscard]] inline constexpr float3 reflect(const float3& i, const float3& n) noexcept {
    return i - 2.f * dot(i, n) * n;
}

// ── Matrix operations ─────────────────────────────────────────────────────────

[[nodiscard]] inline float4x4 transpose(const float4x4& m) noexcept {
    return {
        {m[0][0], m[1][0], m[2][0], m[3][0]},
        {m[0][1], m[1][1], m[2][1], m[3][1]},
        {m[0][2], m[1][2], m[2][2], m[3][2]},
        {m[0][3], m[1][3], m[2][3], m[3][3]},
    };
}

[[nodiscard]] inline float3x3 transpose(const float3x3& m) noexcept {
    return {
        {m[0][0], m[1][0], m[2][0]},
        {m[0][1], m[1][1], m[2][1]},
        {m[0][2], m[1][2], m[2][2]},
    };
}

/// 4×4 matrix inverse via Gauss-Jordan elimination with partial pivoting.
/// Returns identity on a singular matrix.
[[nodiscard]] inline float4x4 inverse(const float4x4& m) noexcept {
    // Build augmented matrix [M | I].  m[row][col] (row-major) maps directly.
    float aug[4][8]{};
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            aug[i][j]   = m[i][j];
            aug[i][j+4] = (i == j) ? 1.f : 0.f;
        }
    }

    for (int col = 0; col < 4; ++col) {
        // Partial pivot: find largest magnitude in this column.
        int pivot = col;
        for (int row = col + 1; row < 4; ++row) {
            if (std::abs(aug[row][col]) > std::abs(aug[pivot][col]))
                pivot = row;
        }
        if (pivot != col) {
            for (int k = 0; k < 8; ++k)
                std::swap(aug[col][k], aug[pivot][k]);
        }
        const float diag = aug[col][col];
        if (std::abs(diag) < 1e-10f)
            return float4x4::identity(); // singular
        const float invDiag = 1.f / diag;
        for (int k = 0; k < 8; ++k) aug[col][k] *= invDiag;

        // Eliminate this column from all other rows.
        for (int row = 0; row < 4; ++row) {
            if (row == col) continue;
            const float f = aug[row][col];
            for (int k = 0; k < 8; ++k) aug[row][k] -= f * aug[col][k];
        }
    }

    // Extract inverse: aug[row][col+4] maps directly to result[row][col].
    float4x4 result;
    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 4; ++j)
            result[i][j] = aug[i][j + 4];
    return result;
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
    // m[row][col]: row 0 = (a,b,c), row 1 = (d,e,f), row 2 = (g,h,k)
    const float a = m[0][0], b = m[0][1], c = m[0][2];
    const float d = m[1][0], e = m[1][1], f = m[1][2];
    const float g = m[2][0], h = m[2][1], k = m[2][2];

    const float det = a*(e*k - f*h) - b*(d*k - f*g) + c*(d*h - e*g);
    if (std::abs(det) < 1e-10f) return float3x3::identity();
    const float inv = 1.f / det;

    // Rows of the inverse (cofactor matrix transposed / det):
    return {
        {  inv*(e*k-f*h), -inv*(b*k-c*h),  inv*(b*f-c*e) },  // row 0
        { -inv*(d*k-f*g),  inv*(a*k-c*g), -inv*(a*f-c*d) },  // row 1
        {  inv*(d*h-e*g), -inv*(a*h-b*g),  inv*(a*e-b*d) },  // row 2
    };
}

/// Normal matrix = transpose(inverse(M)).
/// Equivalent to glm::inverseTranspose(glm::mat3(transform)) — use inverseTranspose(toFloat3x3(t)).
[[nodiscard]] inline float3x3 inverseTranspose(const float3x3& m) noexcept {
    return transpose(inverse(m));
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
/// @deprecated Use toFloat3x3(). GLSL compatibility alias — will be removed in a future version.
[[nodiscard]] inline constexpr float3x3 mat3(const float4x4& m) noexcept { return toFloat3x3(m); }

// ── Pointer access (Vulkan buffer uploads) ────────────────────────────────────
//
// value_ptr returns a const float* to the first element — equivalent to &v.x or &m.
// Useful for passing to Vulkan APIs (vkCmdPushConstants, memcpy, etc.).
// Direct use of &v and &m also works since all types are standard-layout.

[[nodiscard]] inline const float* value_ptr(const float2& v)   noexcept { return &v.x; }
[[nodiscard]] inline const float* value_ptr(const float3& v)   noexcept { return &v.x; }
[[nodiscard]] inline const float* value_ptr(const float4& v)   noexcept { return &v.x; }
[[nodiscard]] inline const float* value_ptr(const float3x3& m) noexcept { return &m.rows[0].x; }
[[nodiscard]] inline const float* value_ptr(const float4x4& m) noexcept { return &m.rows[0].x; }
[[nodiscard]] inline float*       value_ptr(float2& v)          noexcept { return &v.x; }
[[nodiscard]] inline float*       value_ptr(float3& v)          noexcept { return &v.x; }
[[nodiscard]] inline float*       value_ptr(float4& v)          noexcept { return &v.x; }
[[nodiscard]] inline float*       value_ptr(float3x3& m)        noexcept { return &m.rows[0].x; }
[[nodiscard]] inline float*       value_ptr(float4x4& m)        noexcept { return &m.rows[0].x; }

} // namespace sm
