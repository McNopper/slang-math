#ifndef SLANG_MATH_UINT3_HPP
#define SLANG_MATH_UINT3_HPP

#include <cstdint>

#include "operators.hpp"

namespace sm {

/// Three-component unsigned integer vector — mirrors Slang/HLSL `uint3`.
/// Memory layout: x, y, z (12 bytes).
///
/// Arithmetic operators (+, -, *, unary -, and the compound-assignment forms) are
/// provided generically for every `vec` by operators.hpp. Division is intentionally
/// float-only (see float_vec), matching the original uintN behaviour.
struct uint3 {
    std::uint32_t x{}, y{}, z{};

    static constexpr std::int32_t size = 3;
    using value_type = std::uint32_t;

    constexpr uint3() noexcept = default;
    constexpr uint3(std::uint32_t x, std::uint32_t y, std::uint32_t z) noexcept : x(x), y(y), z(z) {}
    constexpr explicit uint3(std::uint32_t s) noexcept : x(s), y(s), z(s) {}

    [[nodiscard]] constexpr std::uint32_t& operator[](std::int32_t i) noexcept { return (&x)[i]; }
    [[nodiscard]] constexpr const std::uint32_t& operator[](std::int32_t i) const noexcept { return (&x)[i]; }

    [[nodiscard]] constexpr bool operator==(const uint3&) const noexcept = default;
};

} // namespace sm
#endif // SLANG_MATH_UINT3_HPP
