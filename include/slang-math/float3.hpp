#ifndef SLANG_MATH_FLOAT3_HPP
#define SLANG_MATH_FLOAT3_HPP

#include <cstdint>

#include "float2.hpp"
#include "operators.hpp"

namespace sm {

/// Three-component float vector — mirrors Slang/HLSL `float3`.
///
/// Memory layout: x, y, z (12 bytes, no padding).
///
/// The r/g/b aliases map to x/y/z respectively — the conventional alternate
/// component spelling for whatever float3 carries: colour channels, texture
/// or data channels, plain vectors alike. Slang supports exactly two swizzle
/// element sets, `xyzw` and `rgba`, and explicitly not GLSL's `stpq` (Slang
/// user guide, "Swizzles") — no s/t/p aliases here. This needs a deliberate ISO-C++
/// extension (anonymous structs): there is no standard spelling that gives two
/// member names for the same bytes without changing the public API. All three
/// family compilers implement it identically; GCC/Clang flag it under
/// -Wpedantic, so the union below carries a narrowly scoped, written exemption
/// (AGENTS.md: fix the finding or justify it in writing).
///
/// Arithmetic operators (+, -, *, /, unary -, and the compound-assignment forms)
/// are provided generically for every `vec` by operators.hpp.
struct float3 {
    // Justified above: layout stays x,y,z (12 bytes, no padding), ABI and
    // behaviour unchanged on MSVC / clang-cl / GCC / Clang.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
    union {
        struct {
            float x, y, z;
        };
        struct {
            float r, g, b;
        }; // r/g/b alias — same bytes as x/y/z
    };
#pragma GCC diagnostic pop

    static constexpr std::int32_t size = 3;
    using value_type = float;

    constexpr float3() noexcept : x{}, y{}, z{} {}
    constexpr float3(float x, float y, float z) noexcept : x(x), y(y), z(z) {}
    constexpr float3(const float2& xy, float z) noexcept : x(xy.x), y(xy.y), z(z) {}
    constexpr explicit float3(float s) noexcept : x(s), y(s), z(s) {}

    [[nodiscard]] constexpr float& operator[](std::int32_t i) noexcept { return (&x)[i]; }
    [[nodiscard]] constexpr const float& operator[](std::int32_t i) const noexcept { return (&x)[i]; }

    [[nodiscard]] constexpr bool operator==(const float3& o) const noexcept { return x == o.x && y == o.y && z == o.z; }
};

} // namespace sm
#endif // SLANG_MATH_FLOAT3_HPP
