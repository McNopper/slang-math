#pragma once

#include "float3.hpp"
#include "float4x4.hpp"
#include "float3x3.hpp"
#include "quaternion.hpp"
#include "functions.hpp"

#include <cmath>

namespace sm {

// ── Transform builders ────────────────────────────────────────────────────────
//
// All functions produce standard mathematical transforms with row-major storage,
// matching Slang's native float4x4 layout.  Upload value_ptr(m) directly to a
// `row_major float4x4` push constant or UBO — no transpose required.
//
// Conventions:
//   - Right-handed coordinate system
//   - Vulkan depth range [0, 1]
//   - `M * v` applies the transform (column-vector form, Slang: mul(M, v))

/// Translation: `translate(m, v)` = M * T(v) — equivalent to glm::translate(m, v).
[[nodiscard]] inline float4x4 translate(const float4x4& m, const float3& v) noexcept {
    // Only the last column (w-component of each row) changes: result[i][3] = dot(row i, v).
    float4x4 result = m;
    result.rows[0].w = m.rows[0].x*v.x + m.rows[0].y*v.y + m.rows[0].z*v.z + m.rows[0].w;
    result.rows[1].w = m.rows[1].x*v.x + m.rows[1].y*v.y + m.rows[1].z*v.z + m.rows[1].w;
    result.rows[2].w = m.rows[2].x*v.x + m.rows[2].y*v.y + m.rows[2].z*v.z + m.rows[2].w;
    result.rows[3].w = m.rows[3].x*v.x + m.rows[3].y*v.y + m.rows[3].z*v.z + m.rows[3].w;
    return result;
}

/// Non-uniform scale: `scale(m, v)` = M * S(v) — equivalent to glm::scale(m, v).
[[nodiscard]] inline float4x4 scale(const float4x4& m, const float3& v) noexcept {
    return {
        {m.rows[0].x*v.x, m.rows[0].y*v.y, m.rows[0].z*v.z, m.rows[0].w},
        {m.rows[1].x*v.x, m.rows[1].y*v.y, m.rows[1].z*v.z, m.rows[1].w},
        {m.rows[2].x*v.x, m.rows[2].y*v.y, m.rows[2].z*v.z, m.rows[2].w},
        {m.rows[3].x*v.x, m.rows[3].y*v.y, m.rows[3].z*v.z, m.rows[3].w},
    };
}

/// Rotation by `angle` radians about `axis`: `rotate(m, angle, axis)` = M * R.
/// Equivalent to glm::rotate(m, angle, axis).
[[nodiscard]] inline float4x4 rotate(const float4x4& m, float angle, const float3& axis) noexcept {
    const float c = std::cos(angle);
    const float s = std::sin(angle);
    const float3 a = normalize(axis);
    const float3 t = (1.f - c) * a; // Rodrigues t = (1-cos) * axis

    // Rodrigues rotation matrix rows (R[i][j] = δ_{ij}·c + t[i]·a[j] + s·[a]×[i][j]):
    const float4x4 rot{
        {c + t.x*a.x,      t.x*a.y - s*a.z,   t.x*a.z + s*a.y,   0.f},  // row 0
        {t.y*a.x + s*a.z,  c + t.y*a.y,        t.y*a.z - s*a.x,   0.f},  // row 1
        {t.z*a.x - s*a.y,  t.z*a.y + s*a.x,   c + t.z*a.z,        0.f},  // row 2
        {0.f,               0.f,                0.f,                1.f},  // row 3
    };
    return m * rot;
}

// ── Camera projection ─────────────────────────────────────────────────────────

/// Right-handed look-at view matrix: equivalent to glm::lookAtRH(eye, center, up).
///
/// Rows 0-2 contain the orthonormal camera basis vectors (right, up, -forward) plus
/// their translation components.  Upload directly — no transpose.
[[nodiscard]] inline float4x4 lookAtRH(const float3& eye,
                                        const float3& center,
                                        const float3& up) noexcept {
    const float3 f = normalize(center - eye); // forward (into scene)
    const float3 r = normalize(cross(f, up)); // right
    const float3 u = cross(r, f);             // re-orthogonalized up

    return {
        { r.x,           r.y,           r.z,          -dot(r, eye) },  // row 0
        { u.x,           u.y,           u.z,          -dot(u, eye) },  // row 1
        {-f.x,          -f.y,          -f.z,           dot(f, eye) },  // row 2
        { 0.f,           0.f,           0.f,           1.f          },  // row 3
    };
}

/// Alias for lookAtRH — glm::lookAt with GLM_FORCE_DEPTH_ZERO_TO_ONE defaults to RH.
[[nodiscard]] inline float4x4 lookAt(const float3& eye,
                                      const float3& center,
                                      const float3& up) noexcept {
    return lookAtRH(eye, center, up);
}

/// Right-handed perspective projection with [0, 1] depth range (Vulkan).
/// Equivalent to glm::perspectiveRH_ZO(fovY, aspect, zNear, zFar).
///
/// fovY    — vertical field of view in radians
/// aspect  — width / height
/// zNear   — near plane distance (> 0)
/// zFar    — far plane distance (> zNear)
[[nodiscard]] inline float4x4 perspectiveRH_ZO(float fovY, float aspect,
                                                float zNear, float zFar) noexcept {
    const float f = 1.f / std::tan(fovY * 0.5f);
    const float d = zFar - zNear;
    return {
        {f / aspect, 0.f,  0.f,              0.f              },  // row 0
        {0.f,        f,    0.f,              0.f              },  // row 1
        {0.f,        0.f, -zFar / d,        -(zFar*zNear)/d   },  // row 2
        {0.f,        0.f, -1.f,              0.f              },  // row 3
    };
}

/// Alias for perspectiveRH_ZO — glm::perspective with GLM_FORCE_DEPTH_ZERO_TO_ONE.
[[nodiscard]] inline float4x4 perspective(float fovY, float aspect,
                                           float zNear, float zFar) noexcept {
    return perspectiveRH_ZO(fovY, aspect, zNear, zFar);
}

// ── Quaternion helpers ────────────────────────────────────────────────────────

/// Convert quaternion to 4×4 rotation matrix: equivalent to glm::mat4_cast(q).
[[nodiscard]] inline float4x4 mat4_cast(const quaternion& q) noexcept {
    return toMatrix(q);
}

/// Build a quaternion representing a rotation of `angle` radians about `axis`.
/// Equivalent to glm::angleAxis(angle, axis).
[[nodiscard]] inline quaternion angleAxis(float angle, const float3& axis) noexcept {
    const float3 a = normalize(axis);
    const float  s = std::sin(angle * 0.5f);
    return {std::cos(angle * 0.5f), a.x * s, a.y * s, a.z * s};
}

// ── Identity template ─────────────────────────────────────────────────────────

/// identity<T>() — equivalent to glm::identity<T>().
template<typename T> [[nodiscard]] inline T identity() noexcept;
template<> [[nodiscard]] inline float4x4   identity<float4x4>()   noexcept { return float4x4::identity(); }
template<> [[nodiscard]] inline float3x3   identity<float3x3>()   noexcept { return float3x3::identity(); }
template<> [[nodiscard]] inline quaternion identity<quaternion>()  noexcept { return quaternion::identity(); }

} // namespace sm
