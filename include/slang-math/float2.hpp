#ifndef SLANG_MATH_FLOAT2_HPP
#define SLANG_MATH_FLOAT2_HPP

#include <cstdint>

#include "operators.hpp"

namespace sm {

/// Two-component float vector — mirrors Slang/HLSL `float2`.
/// Memory layout: x, y (8 bytes, no padding).
///
/// The r/g aliases map to x/y respectively — the conventional alternate
/// component spelling for whatever float2 carries: texture coordinates (uv),
/// colour channels, data channels alike. Slang supports exactly two swizzle
/// element sets, `xyzw` and `rgba`, and explicitly not GLSL's `stpq` (Slang
/// user guide, "Swizzles"), so no s/t alias is provided here. The alias needs a
/// deliberate ISO-C++ extension (anonymous structs): there is no standard
/// spelling that gives two member names for the same bytes without changing
/// the public API. All three family compilers implement it identically;
/// GCC/Clang flag it under -Wpedantic, so the union below carries a narrowly
/// scoped, written exemption (AGENTS.md: fix the finding or justify it in
/// writing).
///
/// Arithmetic operators (+, -, *, /, unary -, and the compound-assignment forms)
/// are provided generically for every `vec` by operators.hpp.
struct float2 {
    // Justified above: layout stays x,y (8 bytes, no padding), ABI and
    // behaviour unchanged on MSVC / clang-cl / GCC / Clang.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
    union {
        struct {
            float x, y;
        };
        struct {
            float r, g;
        }; // r/g alias — same bytes as x/y
    };
#pragma GCC diagnostic pop

    static constexpr std::int32_t size = 2;
    using value_type = float;

    constexpr float2() noexcept : x{}, y{} {}
    constexpr float2(float x, float y) noexcept : x(x), y(y) {}
    constexpr explicit float2(float s) noexcept : x(s), y(s) {}

    [[nodiscard]] constexpr float& operator[](std::int32_t i) noexcept { return (&x)[i]; }
    [[nodiscard]] constexpr const float& operator[](std::int32_t i) const noexcept { return (&x)[i]; }

    [[nodiscard]] constexpr bool operator==(const float2& o) const noexcept { return x == o.x && y == o.y; }
};

} // namespace sm
#endif // SLANG_MATH_FLOAT2_HPP
