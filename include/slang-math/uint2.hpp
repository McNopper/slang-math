#ifndef SLANG_MATH_UINT2_HPP
#define SLANG_MATH_UINT2_HPP

#include <cstdint>

#include "operators.hpp"

namespace sm {

/// Two-component unsigned integer vector — mirrors Slang/HLSL `uint2`.
/// Memory layout: x, y (8 bytes).
///
/// Arithmetic operators (+, -, *, unary -, and the compound-assignment forms) are
/// provided generically for every `vec` by operators.hpp. Division is intentionally
/// float-only (see float_vec), matching the original uintN behaviour.
struct uint2 {
    std::uint32_t x{}, y{};

    static constexpr std::int32_t size = 2;
    using value_type = std::uint32_t;

    constexpr uint2() noexcept = default;
    constexpr uint2(std::uint32_t x, std::uint32_t y) noexcept : x(x), y(y) {}
    constexpr explicit uint2(std::uint32_t s) noexcept : x(s), y(s) {}

    [[nodiscard]] constexpr std::uint32_t& operator[](std::int32_t i) noexcept { return (&x)[i]; }
    [[nodiscard]] constexpr const std::uint32_t& operator[](std::int32_t i) const noexcept { return (&x)[i]; }

    [[nodiscard]] constexpr bool operator==(const uint2&) const noexcept = default;
};

} // namespace sm
#endif // SLANG_MATH_UINT2_HPP
