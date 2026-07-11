#pragma once

#include "float4.hpp"

namespace sm {

/// Row-major 4×4 float matrix — matches Slang/HLSL `float4x4` native memory layout.
///
/// ## Memory layout
///
/// Four row vectors stored contiguously: row0.xyzw, row1.xyzw, row2.xyzw, row3.xyzw.
/// This is the **same layout as Slang shaders declare `float4x4`**, so uploading
/// `value_ptr(m)` (or `&m`) to a `row_major float4x4` push constant/UBO requires
/// **no transpose**.
///
/// ## Indexing
///
/// `operator[](row)` returns row `row` as a float4.
/// `m[row][col]` gives the element at row `row`, column `col` — identical to Slang.
///
/// ## Operators
///
/// `M * v` — column-vector transform: `result[i] = dot(row i, v)`  (Slang: `mul(M, v)`)
/// `v * M` — row-vector form:  `result[j] = Σ_i v[i] * M[i][j]`   (Slang: `mul(v, M)`)
///
/// Identity: `v * M == transpose(M) * v`
struct float4x4 {
    float4 rows[4]{};

    static constexpr std::int32_t size = 4;

    constexpr float4x4() noexcept = default;

    /// Construct from four row vectors (row 0, row 1, row 2, row 3).
    constexpr float4x4(const float4& r0, const float4& r1,
                       const float4& r2, const float4& r3) noexcept
        : rows{r0, r1, r2, r3} {}

    /// Diagonal constructor: `float4x4(1.f)` = identity.
    constexpr explicit float4x4(float s) noexcept
        : rows{{s,0,0,0},{0,s,0,0},{0,0,s,0},{0,0,0,s}} {}

    [[nodiscard]] static constexpr float4x4 identity() noexcept {
        return {{1,0,0,0},{0,1,0,0},{0,0,1,0},{0,0,0,1}};
    }

    /// `m[row]` returns row `row`.  `m[row][col]` = element at (row, col).
    [[nodiscard]] constexpr float4& operator[](std::int32_t row) noexcept { return rows[row]; }
    [[nodiscard]] constexpr const float4& operator[](std::int32_t row) const noexcept { return rows[row]; }

    /// `M * v` — column-vector transform: `result[i] = dot(row i, v)`.  Slang: `mul(M, v)`.
    [[nodiscard]] constexpr float4 operator*(const float4& v) const noexcept {
        return {
            rows[0].x*v.x + rows[0].y*v.y + rows[0].z*v.z + rows[0].w*v.w,
            rows[1].x*v.x + rows[1].y*v.y + rows[1].z*v.z + rows[1].w*v.w,
            rows[2].x*v.x + rows[2].y*v.y + rows[2].z*v.z + rows[2].w*v.w,
            rows[3].x*v.x + rows[3].y*v.y + rows[3].z*v.z + rows[3].w*v.w,
        };
    }

    /// `M * M` — matrix multiply (provided by the free `operator*` template in
    /// functions.hpp, generic over all square matrices).

    [[nodiscard]] constexpr bool operator==(const float4x4&) const noexcept = default;
};

/// `v * M` — row-vector form: `result[j] = Σ_i v[i] * M[i][j]`.  Slang: `mul(v, M)`.
/// Identity: `v * M == transpose(M) * v`.
[[nodiscard]] inline constexpr float4 operator*(const float4& v, const float4x4& m) noexcept {
    return {
        v.x*m[0][0] + v.y*m[1][0] + v.z*m[2][0] + v.w*m[3][0],
        v.x*m[0][1] + v.y*m[1][1] + v.z*m[2][1] + v.w*m[3][1],
        v.x*m[0][2] + v.y*m[1][2] + v.z*m[2][2] + v.w*m[3][2],
        v.x*m[0][3] + v.y*m[1][3] + v.z*m[2][3] + v.w*m[3][3],
    };
}

} // namespace sm
