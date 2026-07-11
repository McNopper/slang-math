#pragma once

#include <cstdint>

namespace sm {

/// Two-component unsigned integer vector — mirrors Slang/HLSL `uint2`.
/// Memory layout: x, y (8 bytes).
struct uint2 {
    uint32_t x{}, y{};

    static constexpr std::int32_t size = 2;
    using value_type = uint32_t;

    constexpr uint2() noexcept = default;
    constexpr uint2(uint32_t x, uint32_t y) noexcept : x(x), y(y) {}
    constexpr explicit uint2(uint32_t s) noexcept : x(s), y(s) {}

    [[nodiscard]] constexpr uint32_t& operator[](std::int32_t i) noexcept { return (&x)[i]; }
    [[nodiscard]] constexpr const uint32_t& operator[](std::int32_t i) const noexcept { return (&x)[i]; }

    [[nodiscard]] constexpr uint2 operator+(const uint2& o) const noexcept { return {x + o.x, y + o.y}; }
    [[nodiscard]] constexpr uint2 operator-(const uint2& o) const noexcept { return {x - o.x, y - o.y}; }
    [[nodiscard]] constexpr uint2 operator*(const uint2& o) const noexcept { return {x * o.x, y * o.y}; }
    [[nodiscard]] constexpr uint2 operator*(uint32_t s) const noexcept { return {x * s, y * s}; }

    [[nodiscard]] constexpr bool operator==(const uint2&) const noexcept = default;
};

} // namespace sm
