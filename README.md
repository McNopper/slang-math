# slang-math

Standalone header-only C++ math library with **Slang/HLSL naming and row-major matrix layout**.

Types mirror the Slang / HLSL built-in math vocabulary (`float2`, `float3`, `float4`,
`float3x3`, `float4x4`, `uint2`, `uint4`, `quaternion`), so host-side C++ and shader code
share the same vocabulary and **no transpose is required** when uploading matrices to a
`row_major float4x4` GPU buffer.

## Matrix layout — the core design decision

Matrices are stored **row-major**: rows are contiguous in memory.

```
float4x4 m{ {1,0,0,tx},   // row 0
             {0,1,0,ty},   // row 1
             {0,0,1,tz},   // row 2
             {0,0,0, 1} }; // row 3
```

`m[row][col]` gives the element at row `row`, column `col` — **identical to Slang/HLSL
indexing**.

### No-transpose guarantee

When a Slang/HLSL shader declares matrices as `row_major` (or uses the `#pragma
pack_matrix(row_major)` directive), the CPU and GPU byte layout are identical.  Upload
`value_ptr(m)` or `&m` directly — no `transpose()` call needed.

| C++ operation | Slang/HLSL equivalent |
|---|---|
| `M * v` | `mul(M, v)` — column-vector transform |
| `v * M` | `mul(v, M)` — row-vector form |
| `v * M == transpose(M) * v` | identity holds for all layouts |

## Types

| sm type        | Slang/HLSL type | Notes                                     |
|----------------|-----------------|-------------------------------------------|
| `sm::float2`   | `float2`        | x, y (8 bytes)                            |
| `sm::float3`   | `float3`        | x/y/z + r/g/b aliases (12 bytes)          |
| `sm::float4`   | `float4`        | x/y/z/w + r/g/b/a aliases (16 bytes)      |
| `sm::uint2`    | `uint2`         | x, y (8 bytes)                            |
| `sm::uint4`    | `uint4`         | x, y, z, w (16 bytes)                     |
| `sm::float3x3` | `float3x3`      | Row-major, 3 × float3 (36 bytes)          |
| `sm::float4x4` | `float4x4`      | Row-major, 4 × float4 (64 bytes)          |
| `sm::quaternion` | —             | (x, y, z, w) storage; `w` = scalar part   |

## Functions

| Function | Description |
|----------|-------------|
| `sm::dot(a, b)` | Dot product |
| `sm::cross(a, b)` | Cross product (float3) |
| `sm::length(v)` | Euclidean length |
| `sm::normalize(v)` | Unit vector |
| `sm::min/max(a, b)` | Component-wise min/max |
| `sm::clamp(v, lo, hi)` | Component-wise clamp |
| `sm::mix(a, b, t)` | Linear interpolation |
| `sm::reflect(i, n)` | Reflection about normal |
| `sm::radians/degrees(x)` | Angle conversion |
| `sm::pi<float>()` | π constant |
| `sm::transpose(m)` | Matrix transpose |
| `sm::inverse(m)` | Matrix inverse (Gauss-Jordan for 4×4, Cramer for 3×3) |
| `sm::inverseTranspose(m)` | `transpose(inverse(m))` — normal matrix |
| `sm::mat3(float4x4)` | Upper-left 3×3 extraction |
| `sm::value_ptr(v/m)` | `const float*` to first element (buffer upload helper) |

## Transform builders

All builders assume a right-handed coordinate system and Vulkan depth range [0, 1].

| Function | Description |
|----------|-------------|
| `sm::lookAtRH(eye, center, up)` | View matrix (RH) |
| `sm::lookAt(eye, center, up)` | Alias for `lookAtRH` |
| `sm::perspectiveRH_ZO(fovY, aspect, near, far)` | Perspective projection (RH, Vulkan ZO) |
| `sm::perspective(fovY, aspect, near, far)` | Alias for `perspectiveRH_ZO` |
| `sm::translate(m, v)` | M × T(v) |
| `sm::rotate(m, angle, axis)` | M × R(angle, axis) — Rodrigues formula |
| `sm::scale(m, v)` | M × S(v) |
| `sm::mat4_cast(q)` | Quaternion → rotation matrix |
| `sm::angleAxis(angle, axis)` | Build rotation quaternion |
| `sm::identity<T>()` | Identity value for any matrix/quaternion type |

## Usage

```cpp
#include <slang-math/slang-math.hpp>  // umbrella — all types + functions
// or à la carte:
#include <slang-math/float3.hpp>
#include <slang-math/functions.hpp>
#include <slang-math/transform.hpp>
```

CMake:
```cmake
add_subdirectory(slang-math)          # or FetchContent
target_link_libraries(mylib PRIVATE slang-math::slang-math)
```

## Build & test

Requires C++20 and CMake 3.25+.

```powershell
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release `
      -DCMAKE_C_COMPILER=clang-cl -DCMAKE_CXX_COMPILER=clang-cl
cmake --build build
cd build; ctest --output-on-failure
```

All 26 tests pass (vectors, matrices, quaternions, transforms, layout verification).

## References

- [Slang language reference](https://shader-slang.com/slang/user-guide/convenience-features.html) — built-in math type naming and row-major convention
- [HLSL matrix packing](https://learn.microsoft.com/en-us/windows/win32/direct3dhlsl/dx-graphics-hlsl-per-component-math) — `row_major` / `column_major` storage qualifiers
- [Rodrigues rotation formula](https://en.wikipedia.org/wiki/Rodrigues%27_rotation_formula) — used in `sm::rotate`
- [Quaternion to rotation matrix](https://www.euclideanspace.com/maths/geometry/rotations/conversions/quaternionToMatrix/) — used in `sm::toMatrix` / `sm::mat4_cast`

