#pragma once

#include "float2.hpp"

namespace sm {

/// Row-major 2×2 float matrix — matches Slang/HLSL `float2x2` native memory layout.
///
/// Storage: two row vectors in memory order: row0.xy, row1.xy.
///
/// Indexing: `operator[](row)` returns row `row` as a float2.
/// `m[row][col]` gives the element at row `row`, column `col` — identical to Slang.
struct float2x2 {
    float2 rows[2]{};

    static constexpr std::int32_t size = 2;

    constexpr float2x2() noexcept = default;

    /// Construct from two row vectors (row 0, row 1).
    constexpr float2x2(const float2& r0, const float2& r1) noexcept
        : rows{r0, r1} {}

    /// Diagonal constructor: `float2x2(1.f)` = identity.
    constexpr explicit float2x2(float s) noexcept
        : rows{{s, 0}, {0, s}} {}

    [[nodiscard]] static constexpr float2x2 identity() noexcept {
        return {{1, 0}, {0, 1}};
    }

    /// `m[row]` returns row `row`.  `m[row][col]` = element at (row, col).
    [[nodiscard]] constexpr float2& operator[](std::int32_t row) noexcept { return rows[row]; }
    [[nodiscard]] constexpr const float2& operator[](std::int32_t row) const noexcept { return rows[row]; }

    /// `M * v` — column-vector transform: `result[i] = dot(row i, v)`.
    [[nodiscard]] constexpr float2 operator*(const float2& v) const noexcept {
        return {
            rows[0].x*v.x + rows[0].y*v.y,
            rows[1].x*v.x + rows[1].y*v.y,
        };
    }

    [[nodiscard]] constexpr bool operator==(const float2x2&) const noexcept = default;
};

} // namespace sm
