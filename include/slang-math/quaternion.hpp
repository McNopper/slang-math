#pragma once

#include "float3.hpp"
#include "float4x4.hpp"

namespace sm {

/// Unit quaternion for 3D rotations.
///
/// Storage order: x, y, z, w — identical to glm::quat.
/// Identity quaternion: w = 1, x = y = z = 0.
struct quaternion {
    float x{}, y{}, z{};
    float w{1.f};

    constexpr quaternion() noexcept = default;
    /// Component constructor: (w, x, y, z) — same argument order as glm::quat(w,x,y,z).
    constexpr quaternion(float w, float x, float y, float z) noexcept
        : x(x), y(y), z(z), w(w) {}

    [[nodiscard]] static constexpr quaternion identity() noexcept {
        return {1.f, 0.f, 0.f, 0.f};
    }

    [[nodiscard]] constexpr bool operator==(const quaternion&) const noexcept = default;
};

/// Rotate a 3D vector by a unit quaternion: q * v * q⁻¹.
/// Equivalent to glm::operator*(quat, vec3).
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
/// Equivalent to glm::mat4_cast(q) — same abstract rotation, row-major storage.
[[nodiscard]] inline float4x4 toMatrix(const quaternion& q) noexcept {
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

} // namespace sm
