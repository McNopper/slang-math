#ifndef SLANG_MATH_FLOAT4_HPP
#define SLANG_MATH_FLOAT4_HPP

#include <cstdint>

#include "float3.hpp"
#include "operators.hpp"

namespace sm {

/// Four-component float vector — mirrors Slang/HLSL `float4`.
///
/// Memory layout: x, y, z, w (16 bytes, no padding).
///
/// The r/g/b/a aliases map to x/y/z/w respectively — the conventional alternate
/// component spelling for whatever float4 carries: colour channels, texture or
/// data channels, plain vectors alike. Slang supports exactly two swizzle
/// element sets, `xyzw` and `rgba`, and explicitly not GLSL's `stpq` (Slang
/// user guide, "Swizzles") — no s/t/p/q aliases here. This needs a deliberate ISO-C++
/// extension (anonymous structs): there is no standard spelling that
/// gives two member names for the same bytes without changing the public API.
/// All three family compilers implement it identically; GCC/Clang flag it
/// under -Wpedantic, so the union below carries a narrowly scoped, written
/// exemption (AGENTS.md: fix the finding or justify it in writing).
///
/// Arithmetic operators (+, -, *, /, unary -, and the compound-assignment forms)
/// are provided generically for every `vec` by operators.hpp.
struct float4 {
    // Justified above: layout stays x,y,z,w (16 bytes, no padding), ABI and
    // behaviour unchanged on MSVC / clang-cl / GCC / Clang.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
    union {
        struct {
            float x, y, z, w;
        };
        struct {
            float r, g, b, a;
        }; // r/g/b/a alias — same bytes as x/y/z/w
    };
#pragma GCC diagnostic pop

    static constexpr std::int32_t size = 4;
    using value_type = float;

    constexpr float4() noexcept : x{}, y{}, z{}, w{} {}
    constexpr float4(float x, float y, float z, float w) noexcept : x(x), y(y), z(z), w(w) {}
    constexpr float4(const float3& xyz, float w) noexcept : x(xyz.x), y(xyz.y), z(xyz.z), w(w) {}
    constexpr float4(const float2& xy, float z, float w) noexcept : x(xy.x), y(xy.y), z(z), w(w) {}
    constexpr explicit float4(float s) noexcept : x(s), y(s), z(s), w(s) {}

    /// Truncate to xyz.
    [[nodiscard]] constexpr explicit operator float3() const noexcept { return {x, y, z}; }

    [[nodiscard]] constexpr float& operator[](std::int32_t i) noexcept { return (&x)[i]; }
    [[nodiscard]] constexpr const float& operator[](std::int32_t i) const noexcept { return (&x)[i]; }

    [[nodiscard]] constexpr bool operator==(const float4& o) const noexcept {
        return x == o.x && y == o.y && z == o.z && w == o.w;
    }
};

} // namespace sm
#endif // SLANG_MATH_FLOAT4_HPP
