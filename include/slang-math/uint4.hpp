#ifndef SLANG_MATH_UINT4_HPP
#define SLANG_MATH_UINT4_HPP

#include <cstdint>

#include "operators.hpp"

namespace sm {

/// Four-component unsigned integer vector — mirrors Slang/HLSL `uint4`.
/// Memory layout: x, y, z, w (16 bytes).
///
/// Arithmetic operators (+, -, *, unary -, and the compound-assignment forms) are
/// provided generically for every `vec` by operators.hpp. Division is intentionally
/// float-only (see float_vec), matching the original uintN behaviour.
struct uint4 {
    std::uint32_t x{}, y{}, z{}, w{};

    static constexpr std::int32_t size = 4;
    using value_type = std::uint32_t;

    constexpr uint4() noexcept = default;
    constexpr uint4(std::uint32_t x, std::uint32_t y, std::uint32_t z, std::uint32_t w) noexcept
        : x(x), y(y), z(z), w(w) {}
    constexpr explicit uint4(std::uint32_t s) noexcept : x(s), y(s), z(s), w(s) {}

    [[nodiscard]] constexpr std::uint32_t& operator[](std::int32_t i) noexcept { return (&x)[i]; }
    [[nodiscard]] constexpr const std::uint32_t& operator[](std::int32_t i) const noexcept { return (&x)[i]; }

    [[nodiscard]] constexpr bool operator==(const uint4&) const noexcept = default;
};

} // namespace sm
#endif // SLANG_MATH_UINT4_HPP
