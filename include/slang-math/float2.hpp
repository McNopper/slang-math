#pragma once

#include <cstdint>

namespace sm {

/// Two-component float vector — mirrors Slang/HLSL `float2`.
/// Memory layout: x, y (8 bytes, no padding).
struct float2 {
    float x{}, y{};

    static constexpr std::int32_t size = 2;
    using value_type = float;

    constexpr float2() noexcept = default;
    constexpr float2(float x, float y) noexcept : x(x), y(y) {}
    constexpr explicit float2(float s) noexcept : x(s), y(s) {}

    [[nodiscard]] constexpr float& operator[](std::int32_t i) noexcept { return (&x)[i]; }
    [[nodiscard]] constexpr const float& operator[](std::int32_t i) const noexcept { return (&x)[i]; }

    [[nodiscard]] constexpr float2 operator+(const float2& o) const noexcept { return {x + o.x, y + o.y}; }
    [[nodiscard]] constexpr float2 operator-(const float2& o) const noexcept { return {x - o.x, y - o.y}; }
    [[nodiscard]] constexpr float2 operator*(const float2& o) const noexcept { return {x * o.x, y * o.y}; }
    [[nodiscard]] constexpr float2 operator/(const float2& o) const noexcept { return {x / o.x, y / o.y}; }
    [[nodiscard]] constexpr float2 operator+(float s) const noexcept { return {x + s, y + s}; }
    [[nodiscard]] constexpr float2 operator-(float s) const noexcept { return {x - s, y - s}; }
    [[nodiscard]] constexpr float2 operator*(float s) const noexcept { return {x * s, y * s}; }
    [[nodiscard]] constexpr float2 operator/(float s) const noexcept { return {x / s, y / s}; }
    [[nodiscard]] constexpr float2 operator-() const noexcept { return {-x, -y}; }

    constexpr float2& operator+=(const float2& o) noexcept { x += o.x; y += o.y; return *this; }
    constexpr float2& operator-=(const float2& o) noexcept { x -= o.x; y -= o.y; return *this; }
    constexpr float2& operator*=(const float2& o) noexcept { x *= o.x; y *= o.y; return *this; }
    constexpr float2& operator/=(const float2& o) noexcept { x /= o.x; y /= o.y; return *this; }
    constexpr float2& operator*=(float s) noexcept { x *= s; y *= s; return *this; }
    constexpr float2& operator/=(float s) noexcept { x /= s; y /= s; return *this; }

    [[nodiscard]] constexpr bool operator==(const float2&) const noexcept = default;
};

[[nodiscard]] inline constexpr float2 operator*(float s, const float2& v) noexcept { return v * s; }
[[nodiscard]] inline constexpr float2 operator+(float s, const float2& v) noexcept { return v + s; }

} // namespace sm
