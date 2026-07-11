#pragma once

#include "float3.hpp"

namespace sm {

/// Row-major 3×3 float matrix — matches Slang/HLSL `float3x3` native memory layout.
///
/// Storage: three row vectors in memory order: row0.xyz, row1.xyz, row2.xyz.
///
/// Indexing: `operator[](row)` returns row `row` as a float3.
/// `m[row][col]` gives the element at row `row`, column `col` — identical to Slang.
struct float3x3 {
    float3 rows[3]{};

    static constexpr std::int32_t size = 3;

    constexpr float3x3() noexcept = default;

    /// Construct from three row vectors (row 0, row 1, row 2).
    constexpr float3x3(const float3& r0, const float3& r1, const float3& r2) noexcept
        : rows{r0, r1, r2} {}

    /// Diagonal constructor: `float3x3(1.f)` = identity.
    constexpr explicit float3x3(float s) noexcept
        : rows{{s, 0, 0}, {0, s, 0}, {0, 0, s}} {}

    [[nodiscard]] static constexpr float3x3 identity() noexcept {
        return {{1, 0, 0}, {0, 1, 0}, {0, 0, 1}};
    }

    /// `m[row]` returns row `row`.  `m[row][col]` = element at (row, col).
    [[nodiscard]] constexpr float3& operator[](std::int32_t row) noexcept { return rows[row]; }
    [[nodiscard]] constexpr const float3& operator[](std::int32_t row) const noexcept { return rows[row]; }

    /// `M * v` — column-vector transform: `result[i] = dot(row i, v)`.
    [[nodiscard]] constexpr float3 operator*(const float3& v) const noexcept {
        return {
            rows[0].x*v.x + rows[0].y*v.y + rows[0].z*v.z,
            rows[1].x*v.x + rows[1].y*v.y + rows[1].z*v.z,
            rows[2].x*v.x + rows[2].y*v.y + rows[2].z*v.z,
        };
    }

    /// `M * M` — matrix multiply (provided by the free `operator*` template in
    /// functions.hpp, generic over all square matrices).

    [[nodiscard]] constexpr bool operator==(const float3x3&) const noexcept = default;
};

} // namespace sm
