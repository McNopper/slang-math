#pragma once

#include <cstdint>

namespace sm {

/// Three-component unsigned integer vector — mirrors Slang/HLSL `uint3`.
/// Memory layout: x, y, z (12 bytes).
struct uint3 {
    uint32_t x{}, y{}, z{};

    static constexpr std::int32_t size = 3;
    using value_type = uint32_t;

    constexpr uint3() noexcept = default;
    constexpr uint3(uint32_t x, uint32_t y, uint32_t z) noexcept : x(x), y(y), z(z) {}
    constexpr explicit uint3(uint32_t s) noexcept : x(s), y(s), z(s) {}

    [[nodiscard]] constexpr uint32_t& operator[](std::int32_t i) noexcept { return (&x)[i]; }
    [[nodiscard]] constexpr const uint32_t& operator[](std::int32_t i) const noexcept { return (&x)[i]; }

    [[nodiscard]] constexpr uint3 operator+(const uint3& o) const noexcept { return {x + o.x, y + o.y, z + o.z}; }
    [[nodiscard]] constexpr uint3 operator-(const uint3& o) const noexcept { return {x - o.x, y - o.y, z - o.z}; }
    [[nodiscard]] constexpr uint3 operator*(const uint3& o) const noexcept { return {x * o.x, y * o.y, z * o.z}; }
    [[nodiscard]] constexpr uint3 operator*(uint32_t s) const noexcept { return {x * s, y * s, z * s}; }

    [[nodiscard]] constexpr bool operator==(const uint3&) const noexcept = default;
};

} // namespace sm
