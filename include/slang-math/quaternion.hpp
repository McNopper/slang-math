#pragma once

#include "float3.hpp"
#include "float4x4.hpp"

#include <cmath>

namespace sm {

/// Unit quaternion for 3D rotations.
///
/// Storage order: x, y, z, w.
/// Identity quaternion: w = 1, x = y = z = 0.
struct quaternion {
    float x{}, y{}, z{};
    float w{1.f};

    constexpr quaternion() noexcept = default;
    /// Component constructor: (x, y, z, w) — matches member declaration order and Slang float4 style.
    constexpr quaternion(float x, float y, float z, float w) noexcept
        : x(x), y(y), z(z), w(w) {}

    [[nodiscard]] static constexpr quaternion identity() noexcept {
        return {0.f, 0.f, 0.f, 1.f};
    }

    [[nodiscard]] constexpr bool operator==(const quaternion&) const noexcept = default;
};

/// Rotate a 3D vector by a unit quaternion: q * v * q⁻¹.
[[nodiscard]] inline constexpr float3 operator*(const quaternion& q, const float3& v) noexcept {
    // Rodrigues' formula: v' = v + 2w(qv × v) + 2(qv × (qv × v))
    const float3 qv{q.x, q.y, q.z};
    const float3 t{
        2.f * (qv.y * v.z - qv.z * v.y),
        2.f * (qv.z * v.x - qv.x * v.z),
        2.f * (qv.x * v.y - qv.y * v.x)
    };
    return {
        v.x + q.w * t.x + (qv.y * t.z - qv.z * t.y),
        v.y + q.w * t.y + (qv.z * t.x - qv.x * t.z),
        v.z + q.w * t.z + (qv.x * t.y - qv.y * t.x)
    };
}

/// Convert a unit quaternion to a 4×4 row-major rotation matrix.
/// Name-aligned with `toFloat3x3`.  Same abstract rotation, row-major storage.
[[nodiscard]] inline float4x4 toFloat4x4(const quaternion& q) noexcept {
    const float x2 = q.x*q.x, y2 = q.y*q.y, z2 = q.z*q.z;
    const float xy = q.x*q.y, xz = q.x*q.z, yz = q.y*q.z;
    const float wx = q.w*q.x, wy = q.w*q.y, wz = q.w*q.z;
    // Rotation matrix rows (R[i][j] = δ_{ij} - 2*...):
    return {
        {1.f-2.f*(y2+z2),   2.f*(xy-wz),    2.f*(xz+wy),   0.f},  // row 0
        {  2.f*(xy+wz), 1.f-2.f*(x2+z2),    2.f*(yz-wx),   0.f},  // row 1
        {  2.f*(xz-wy),   2.f*(yz+wx),  1.f-2.f*(x2+y2),   0.f},  // row 2
        {           0.f,           0.f,            0.f,     1.f},  // row 3
    };
}

// ── Quaternion algebra ────────────────────────────────────────────────────────
//
// Slang/HLSL has no quaternion type, so there is no canonical intrinsic name for
// these.  `dot` / `normalize` reuse the vector-intrinsic spelling for consistency;
// `conjugate` / `slerp` / `operator*` follow standard quaternion-math conventions.

/// Quaternion dot product (4D): a·b over (x, y, z, w).
[[nodiscard]] inline constexpr float dot(const quaternion& a, const quaternion& b) noexcept {
    return a.x*b.x + a.y*b.y + a.z*b.z + a.w*b.w;
}

/// Return the unit quaternion in the direction of `q` (identity if degenerate).
[[nodiscard]] inline quaternion normalize(const quaternion& q) noexcept {
    const float len = std::sqrt(dot(q, q));
    if (len <= 0.f) return quaternion::identity();
    const float inv = 1.f / len;
    return {q.x*inv, q.y*inv, q.z*inv, q.w*inv};
}

/// Quaternion conjugate: negate the vector part.  For a unit quaternion this is the
/// inverse rotation (q⁻¹ = conjugate(q)).
[[nodiscard]] inline constexpr quaternion conjugate(const quaternion& q) noexcept {
    return {-q.x, -q.y, -q.z, q.w};
}

/// Hamilton product `a * b` — composes rotations: `(a * b)` applies `b` then `a`,
/// i.e. `(a * b) * v == a * (b * v)`.
[[nodiscard]] inline constexpr quaternion operator*(const quaternion& a, const quaternion& b) noexcept {
    return {
        a.w*b.x + a.x*b.w + a.y*b.z - a.z*b.y, // x
        a.w*b.y - a.x*b.z + a.y*b.w + a.z*b.x, // y
        a.w*b.z + a.x*b.y - a.y*b.x + a.z*b.w, // z
        a.w*b.w - a.x*b.x - a.y*b.y - a.z*b.z, // w
    };
}

/// Spherical linear interpolation between unit quaternions along the shortest arc.
/// Falls back to normalized lerp when the inputs are nearly parallel.
[[nodiscard]] inline quaternion slerp(const quaternion& a, quaternion b, float t) noexcept {
    float cosTheta = dot(a, b);
    // Take the shortest path: flip b if the quaternions are more than 90° apart.
    if (cosTheta < 0.f) {
        b = {-b.x, -b.y, -b.z, -b.w};
        cosTheta = -cosTheta;
    }
    // Nearly parallel — normalized lerp avoids division by ~0.
    if (cosTheta > 0.9995f) {
        return normalize(quaternion{
            a.x + t*(b.x - a.x),
            a.y + t*(b.y - a.y),
            a.z + t*(b.z - a.z),
            a.w + t*(b.w - a.w),
        });
    }
    const float theta = std::acos(cosTheta);
    const float sinTheta = std::sin(theta);
    const float wa = std::sin((1.f - t) * theta) / sinTheta;
    const float wb = std::sin(t * theta) / sinTheta;
    return {
        wa*a.x + wb*b.x,
        wa*a.y + wb*b.y,
        wa*a.z + wb*b.z,
        wa*a.w + wb*b.w,
    };
}

} // namespace sm
