#pragma once

#include <cstdint>

namespace sm {

/// Four-component unsigned integer vector — mirrors Slang/HLSL `uint4`.
/// Memory layout: x, y, z, w (16 bytes) — matches glm::uvec4.
struct uint4 {
    uint32_t x{}, y{}, z{}, w{};

    constexpr uint4() noexcept = default;
    constexpr uint4(uint32_t x, uint32_t y, uint32_t z, uint32_t w) noexcept
        : x(x), y(y), z(z), w(w) {}
    constexpr explicit uint4(uint32_t s) noexcept : x(s), y(s), z(s), w(s) {}

    [[nodiscard]] constexpr uint32_t& operator[](int i) noexcept { return (&x)[i]; }
    [[nodiscard]] constexpr const uint32_t& operator[](int i) const noexcept { return (&x)[i]; }

    [[nodiscard]] constexpr bool operator==(const uint4&) const noexcept = default;
};

} // namespace sm
