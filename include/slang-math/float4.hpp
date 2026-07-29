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
/// The r/g/b/a aliases map to x/y/z/w respectively.
///
/// Arithmetic operators (+, -, *, /, unary -, and the compound-assignment forms)
/// are provided generically for every `vec` by operators.hpp.
struct float4 {
    union {
        struct {
            float x, y, z, w;
        };
        struct {
            float r, g, b, a;
        }; // colour alias — same bytes as x/y/z/w
    };

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
