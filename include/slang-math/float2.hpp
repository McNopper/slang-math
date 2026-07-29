#ifndef SLANG_MATH_FLOAT2_HPP
#define SLANG_MATH_FLOAT2_HPP

#include <cstdint>

#include "operators.hpp"

namespace sm {

/// Two-component float vector — mirrors Slang/HLSL `float2`.
/// Memory layout: x, y (8 bytes, no padding).
///
/// Arithmetic operators (+, -, *, /, unary -, and the compound-assignment forms)
/// are provided generically for every `vec` by operators.hpp.
struct float2 {
    float x{}, y{};

    static constexpr std::int32_t size = 2;
    using value_type = float;

    constexpr float2() noexcept = default;
    constexpr float2(float x, float y) noexcept : x(x), y(y) {}
    constexpr explicit float2(float s) noexcept : x(s), y(s) {}

    [[nodiscard]] constexpr float& operator[](std::int32_t i) noexcept { return (&x)[i]; }
    [[nodiscard]] constexpr const float& operator[](std::int32_t i) const noexcept { return (&x)[i]; }

    [[nodiscard]] constexpr bool operator==(const float2&) const noexcept = default;
};

} // namespace sm
#endif // SLANG_MATH_FLOAT2_HPP
