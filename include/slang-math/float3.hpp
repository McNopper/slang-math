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
/// The r/g/b aliases map to x/y/z respectively (compiler extension, universally
/// supported by MSVC/GCC/Clang) so colour-math code can use either spelling.
///
/// Arithmetic operators (+, -, *, /, unary -, and the compound-assignment forms)
/// are provided generically for every `vec` by operators.hpp.
struct float3 {
    union {
        struct {
            float x, y, z;
        };
        struct {
            float r, g, b;
        }; // colour alias — same bytes as x/y/z
    };

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
