#pragma once

#include "float2.hpp"

namespace sm {

/// Three-component float vector — mirrors Slang/HLSL `float3`.
///
/// Memory layout: x, y, z (12 bytes, no padding) — matches glm::vec3.
///
/// The r/g/b aliases map to x/y/z respectively (compiler extension, universally
/// supported by MSVC/GCC/Clang) so colour-math code can use either spelling.
struct float3 {
    union {
        struct { float x, y, z; };
        struct { float r, g, b; }; // colour alias — same bytes as x/y/z
    };

    constexpr float3() noexcept : x{}, y{}, z{} {}
    constexpr float3(float x, float y, float z) noexcept : x(x), y(y), z(z) {}
    constexpr float3(const float2& xy, float z) noexcept : x(xy.x), y(xy.y), z(z) {}
    constexpr explicit float3(float s) noexcept : x(s), y(s), z(s) {}

    [[nodiscard]] constexpr float& operator[](int i) noexcept { return (&x)[i]; }
    [[nodiscard]] constexpr const float& operator[](int i) const noexcept { return (&x)[i]; }

    [[nodiscard]] constexpr float3 operator+(const float3& o) const noexcept { return {x + o.x, y + o.y, z + o.z}; }
    [[nodiscard]] constexpr float3 operator-(const float3& o) const noexcept { return {x - o.x, y - o.y, z - o.z}; }
    [[nodiscard]] constexpr float3 operator*(const float3& o) const noexcept { return {x * o.x, y * o.y, z * o.z}; }
    [[nodiscard]] constexpr float3 operator/(const float3& o) const noexcept { return {x / o.x, y / o.y, z / o.z}; }
    [[nodiscard]] constexpr float3 operator+(float s) const noexcept { return {x + s, y + s, z + s}; }
    [[nodiscard]] constexpr float3 operator-(float s) const noexcept { return {x - s, y - s, z - s}; }
    [[nodiscard]] constexpr float3 operator*(float s) const noexcept { return {x * s, y * s, z * s}; }
    [[nodiscard]] constexpr float3 operator/(float s) const noexcept { return {x / s, y / s, z / s}; }
    [[nodiscard]] constexpr float3 operator-() const noexcept { return {-x, -y, -z}; }

    constexpr float3& operator+=(const float3& o) noexcept { x += o.x; y += o.y; z += o.z; return *this; }
    constexpr float3& operator-=(const float3& o) noexcept { x -= o.x; y -= o.y; z -= o.z; return *this; }
    constexpr float3& operator*=(const float3& o) noexcept { x *= o.x; y *= o.y; z *= o.z; return *this; }
    constexpr float3& operator/=(const float3& o) noexcept { x /= o.x; y /= o.y; z /= o.z; return *this; }
    constexpr float3& operator*=(float s) noexcept { x *= s; y *= s; z *= s; return *this; }
    constexpr float3& operator/=(float s) noexcept { x /= s; y /= s; z /= s; return *this; }

    [[nodiscard]] constexpr bool operator==(const float3& o) const noexcept {
        return x == o.x && y == o.y && z == o.z;
    }
};

[[nodiscard]] inline constexpr float3 operator*(float s, const float3& v) noexcept { return v * s; }
[[nodiscard]] inline constexpr float3 operator+(float s, const float3& v) noexcept { return v + s; }
[[nodiscard]] inline constexpr float3 operator/(float s, const float3& v) noexcept {
    return {s / v.x, s / v.y, s / v.z};
}

} // namespace sm
