# AGENTS.md — slang-math

Quick-start context for AI agents so basic facts don't have to be rediscovered.

**Outstanding work, governance (definition of done + guardrails) and release history: see
[`PLAN.md`](PLAN.md).**

## What this repo is

**slang-math** is a standalone, **header-only C++20** math library shared by the four-repo
rendering pipeline. It provides the vector / matrix / quaternion types and free functions used
by both host-side C++ and (by naming parity) Slang/HLSL shaders.

Pipeline (dependency direction):

```mermaid
flowchart LR
    SM["<b>slang-math</b><br/>math (this repo)"] --> A["Aether<br/>file format"]
    A --> H["Harmonia<br/>shared Vulkan lib"]
    SM --> H
    H --> Hy["Hyperion<br/>path tracer · ground truth"]
    H --> T["Theia<br/>real-time renderer"]
```

Consumed via CMake **FetchContent** (`GIT_TAG vX.Y.Z`). Downstream builds override the pin with
`-DFETCHCONTENT_SOURCE_DIR_SLANG_MATH=<path-to-slang-math>` to test local changes
against unpushed tags.

## Core design decision — row-major storage

Matrices are stored **row-major** (rows contiguous), matching Slang/HLSL's native `float4x4`
layout. `m[row][col]` indexing is identical to Slang. Upload `value_ptr(m)` / `&m` directly to a
`row_major float4x4` GPU buffer — **no transpose**. `M * v` == Slang `mul(M, v)` (column-vector);
`v * M` == `mul(v, M)` (row-vector).

## Naming conventions (enforced — do not regress)

- **One canonical name per operation. NO aliases.** Do not re-add `mix`, `mat3`, `mat4_cast`,
  `toMatrix`, `lookAtRH`, `perspectiveRH_ZO`, `inverseLookAtRH`, `inversePerspectiveRH_ZO`, etc.
- **Align to the Slang/HLSL intrinsic spelling** where one exists: `dot`, `cross`, `normalize`,
  `length`, `lerp`, `clamp`, `reflect`, `transpose`, `inverse`, `radians`/`degrees`.
- **Casts use `toFloatNxN`:** `toFloat3x3(float4x4)`, `toFloat4x4(quaternion)`.
- **Quaternion is `(x, y, z, w)`; identity `{0,0,0,1}`** (matches Slang `float4`, NOT glm wxyz).
  Algebra: `dot`, `normalize`, `conjugate`, `operator*` (Hamilton, composes rotations), `slerp`.
- **Single coordinate convention → no `RH`/`ZO` suffix.** Only right-handed + Vulkan zero-to-one
  depth is shipped, so `lookAt` / `perspective` (+ `inverseLookAt` / `inversePerspective`) carry
  no suffix.

## Type-set policy

slang-math is a **complete, consistent, standalone** package — not just the subset the
pipeline currently consumes. Ship the full set for every supported scalar:

- float vectors: `float2/3/4` · integer vectors: `uint2/3/4` · square matrices: `float2x2/3x3/4x4`.
- **Free functions are generic over C++20 concepts** (`functions.hpp`): `vec` (any component
  vector — `float2/3/4` AND `uint2/3/4`, element-agnostic) carries the type-agnostic ops
  `dot`, `min`, `max`, `clamp`, `value_ptr`; `float_vec` (refines `vec`, `value_type == float`)
  carries the real-valued ops `length`, `normalize`, `lerp`, `abs`, `sqrt`, `exp`, `cos`, `sin`,
  `tan`, `asin`, `acos`, `atan`, `atan2`, `log`, `pow` (vector and vector-exponent forms),
  `distance`, `reflect`, `faceforward`, `refract`, `floor`, `ceil`, `trunc`, `round`, `frac`,
  `fmod`, `rsqrt`, `fma`, `step`, `smoothstep`, `exp2`, `log2`, `log10`, `any_nan`, `any_inf`,
  `all_finite`; `square_mat` (`float2x2/3x3/4x4`) carries `transpose` and
  matrix×matrix `operator*`. No float3-only special cases — a new scalar's vectors inherit the
  `vec` ops automatically.
- **Scalar/vector availability follows C-library collisions** (verified against MSVC/UCRT):
  collision-free names ship **scalar + vector pairs** (`sign`, `frac`, `rsqrt`, `step`, plus
  `clamp`, `lerp`, `saturate`, `smoothstep`). Names colliding with `<cmath>`'s global float
  overloads are **vector-only** — scalar callers use `std::` (`floor`, `ceil`, `trunc`,
  `round`, `fmod`, `fma`, `exp2`, `log2`, `log10`, the trig set `sin`/`tan`/`asin`/`acos`/
  `atan`/`atan2`, and the pre-existing `sqrt`, `exp`, `cos`, `log`). The C-runtime-owned
  names `isnan`/`isinf`/`isfinite` are unshippable even for vectors (the UCRT declares
  unconstrained global templates that deduce on any type); the classification trio ships
  under contract names `any_nan` / `any_inf` / `all_finite`.
- **Sized integers only** — use `std::int32_t` / `std::uint32_t` (never bare `int`/`unsigned`)
  for stored members, dimensions (`static constexpr std::int32_t size`), and index/signature
  types. Vector elements keep their natural width (`float`; `uint32_t` for `uintN`).
- **When a new scalar type is first needed (e.g. signed `int`), add the FULL set at once**
  (`int2`, `int3`, `int4`) — never ship a scalar with a partial vector set
  (no `int2`+`int4` without `int3`).
- Matrix `transpose` and matrix×matrix `operator*` are generic over `square_mat`
  (`float2x2/3x3/4x4`); `inverse` stays size-specialized (closed-form 2×2, Cramer 3×3,
  Gauss-Jordan 4×4) — do not collapse to a single generic inverse.

## Layout of the headers

`include/slang-math/`: `float2/3/4.hpp`, `uint2/3/4.hpp`, `float2x2.hpp`, `float3x3.hpp`,
`float4x4.hpp`, `quaternion.hpp`, `functions.hpp` (free functions), `transform.hpp`
(camera/projection builders), `slang-math.hpp` (umbrella). Add à-la-carte includes to the
umbrella when introducing a new header.

## Build & test

```powershell
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release `
      -DCMAKE_C_COMPILER=clang-cl -DCMAKE_CXX_COMPILER=clang-cl
cmake --build build
ctest --test-dir build --output-on-failure
```

Equivalent preset flow (Ninja + Release + clang-cl, no vcpkg — header-only, no deps):
`cmake --preset win` / `cmake --build --preset win` / `ctest --preset win`.

**Lint (CI):** `.github/workflows/build.yml` runs a **blocking** `lint (Q2-Q6)` job
on ubuntu-latest: clang-format (pinned 22.1.3 via pip - the local dev version; a
different version reformats differently and fails), cppcheck (explicit source-file
scan of `src/` + `include/`, gate = no `error:` severity), ruff over `tools/`
(contract: root `ruff.toml`, identical in all five family repos; findings stay at
zero - fix code, do not grow ignores), bandit (accepted tool-script patterns in the
`--skip` list), actionlint + yamllint on the workflows. Local pre-flight: run the
same commands from the repo root before pushing a lint-affecting change.

**Static analysis:** `python tools/check_tidy.py` — parallel clang-tidy over
`build/compile_commands.json`, classified per `.clang-tidy`'s WarningsAsErrors contract
(clang-diagnostic/clang-analyzer/bugprone fail the run; modernize/performance/portability
are report-only). Also registered as ctest `test_tidy` (label `analysis`; the fast test loop is `ctest -LE analysis`; skips when
clang-tidy, Python3 or the database is missing).
**Architecture gate:** `ARCHITECTURE.md` is the machine-checked architecture contract
(the document is the contract, the include graph is measured
against it). ctest `test_arch` runs the vendored `tools/check_arch.py` engine (label
`analysis`; local; not wired into CI). Contract:
slang-math is a flat single-module header library, so the gate enforces the
one architecture contract that applies: **no cyclic `#include` dependencies** (file-level
SCC detection; partial/deferred cycles that merely compile still fail). slang-math is the family root — nothing downstream may ever be included.
**Coverage lane (Clang/GCC configures only):** configure a separate build with
`-DSLANGMATH_COVERAGE=ON` (e.g. `-B build/coverage`), then run `python tools/coverage.py` - it
builds the instrumented tree, runs the tests with `LLVM_PROFILE_FILE` set, merges the
profiles and prints the llvm-cov report for `src/` (gcc builds report via gcov; exit 77
when the LLVM tools are missing). Wired into CI as a report-only `coverage (Q1)` job (ubuntu-26.04, clang + llvm tools, `continue-on-error` while it stabilizes) - the CPU suite runs in CI, so the report measures real coverage.

**Sanitizer lane (Clang/GCC configures only):** `-DSLANGMATH_SANITIZER=address|undefined|thread`. Host-side FP is deterministic
(`/fp:strict` / `-ffp-contract=off -fno-fast-math`).

Tests (`tests/test_slang_math.cpp`, GoogleTest via FetchContent) are the contract: every function
should have a round-trip or identity test. Add a test alongside any new function.

## Conventions

- Commit, but do **not** push unless asked.
- Header-only: no `.cpp`, no external dependencies. Keep it `constexpr`-friendly where practical.
- API additions are a minor version bump; breaking renames coordinate with downstream repos
  (migrate all call sites in Aether/Harmonia/Hyperion/Theia in the same change).
