#pragma once

#include <cstdint>

namespace sm {

/// Four-component unsigned integer vector — mirrors Slang/HLSL `uint4`.
/// Memory layout: x, y, z, w (16 bytes).
struct uint4 {
    uint32_t x{}, y{}, z{}, w{};

    static constexpr std::int32_t size = 4;
    using value_type = uint32_t;

    constexpr uint4() noexcept = default;
    constexpr uint4(uint32_t x, uint32_t y, uint32_t z, uint32_t w) noexcept
        : x(x), y(y), z(z), w(w) {}
    constexpr explicit uint4(uint32_t s) noexcept : x(s), y(s), z(s), w(s) {}

    [[nodiscard]] constexpr uint32_t& operator[](std::int32_t i) noexcept { return (&x)[i]; }
    [[nodiscard]] constexpr const uint32_t& operator[](std::int32_t i) const noexcept { return (&x)[i]; }

    [[nodiscard]] constexpr uint4 operator+(const uint4& o) const noexcept { return {x + o.x, y + o.y, z + o.z, w + o.w}; }
    [[nodiscard]] constexpr uint4 operator-(const uint4& o) const noexcept { return {x - o.x, y - o.y, z - o.z, w - o.w}; }
    [[nodiscard]] constexpr uint4 operator*(const uint4& o) const noexcept { return {x * o.x, y * o.y, z * o.z, w * o.w}; }
    [[nodiscard]] constexpr uint4 operator*(uint32_t s) const noexcept { return {x * s, y * s, z * s, w * s}; }

    [[nodiscard]] constexpr bool operator==(const uint4&) const noexcept = default;
};

} // namespace sm
