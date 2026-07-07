#include <slang-math/slang-math.hpp>

#include <cmath>
#include <gtest/gtest.h>

using namespace sm;

// ── Helpers ──────────────────────────────────────────────────────────────────

static constexpr float kEps = 1e-5f;

static bool near(float a, float b, float eps = kEps) { return std::abs(a - b) <= eps; }
static bool near(const float2& a, const float2& b, float eps = kEps) {
    return near(a.x, b.x, eps) && near(a.y, b.y, eps);
}
static bool near(const float3& a, const float3& b, float eps = kEps) {
    return near(a.x, b.x, eps) && near(a.y, b.y, eps) && near(a.z, b.z, eps);
}
static bool near(const float4& a, const float4& b, float eps = kEps) {
    return near(a.x, b.x, eps) && near(a.y, b.y, eps) &&
           near(a.z, b.z, eps) && near(a.w, b.w, eps);
}
static bool near(const float4x4& a, const float4x4& b, float eps = kEps) {
    for (int c = 0; c < 4; ++c)
        if (!near(a[c], b[c], eps)) return false;
    return true;
}

// ── float2 ───────────────────────────────────────────────────────────────────

TEST(Float2, BasicArithmetic) {
    float2 a{1, 2}, b{3, 4};
    EXPECT_EQ(a + b, (float2{4, 6}));
    EXPECT_EQ(a - b, (float2{-2, -2}));
    EXPECT_EQ(a * b, (float2{3, 8}));
    EXPECT_EQ(a * 2.f, (float2{2, 4}));
    EXPECT_EQ(2.f * a, (float2{2, 4}));
    EXPECT_EQ(-a,      (float2{-1, -2}));
}

TEST(Float2, Index) {
    float2 v{5, 7};
    EXPECT_EQ(v[0], 5.f);
    EXPECT_EQ(v[1], 7.f);
}

// ── float3 ───────────────────────────────────────────────────────────────────

TEST(Float3, RgbAlias) {
    float3 c{0.1f, 0.2f, 0.3f};
    EXPECT_EQ(c.r, c.x);
    EXPECT_EQ(c.g, c.y);
    EXPECT_EQ(c.b, c.z);
}

TEST(Float3, BasicArithmetic) {
    float3 a{1, 2, 3}, b{4, 5, 6};
    EXPECT_EQ(a + b, (float3{5, 7, 9}));
    EXPECT_EQ(a * 3.f, (float3{3, 6, 9}));
    EXPECT_EQ(-a,      (float3{-1, -2, -3}));
}

// ── float4 ───────────────────────────────────────────────────────────────────

TEST(Float4, RgbaAlias) {
    float4 c{0.1f, 0.2f, 0.3f, 0.4f};
    EXPECT_EQ(c.r, c.x);
    EXPECT_EQ(c.g, c.y);
    EXPECT_EQ(c.b, c.z);
    EXPECT_EQ(c.a, c.w);
}

TEST(Float4, FromFloat3) {
    float3 v{1, 2, 3};
    float4 w{v, 1.f};
    EXPECT_EQ(w, (float4{1, 2, 3, 1}));
    float3 back = static_cast<float3>(w);
    EXPECT_EQ(back, v);
}

// ── float4x4 ─────────────────────────────────────────────────────────────────

TEST(Float4x4, IdentityDiagonal) {
    float4x4 m{1.f};
    for (int row = 0; row < 4; ++row)
        for (int col = 0; col < 4; ++col)
            EXPECT_EQ(m[row][col], (row == col) ? 1.f : 0.f);
}

TEST(Float4x4, MatrixVectorMultiply) {
    // Identity * v = v
    float4x4 id{1.f};
    float4 v{1, 2, 3, 4};
    EXPECT_EQ(id * v, v);
}

TEST(Float4x4, RowVectorMultiply) {
    // v * identity = v
    float4x4 id{1.f};
    float4 v{1, 2, 3, 4};
    EXPECT_EQ(v * id, v);
}

TEST(Float4x4, Transpose) {
    float4x4 m{
        {1, 2, 3, 4},
        {5, 6, 7, 8},
        {9, 10, 11, 12},
        {13, 14, 15, 16},
    };
    float4x4 t = transpose(m);
    // t[row][col] = m[col][row]  (row-major indexing, same formula as column-major)
    for (int r = 0; r < 4; ++r)
        for (int c = 0; c < 4; ++c)
            EXPECT_EQ(t[r][c], m[c][r]);
}

TEST(Float4x4, InverseIdentity) {
    // inverse(I) == I
    EXPECT_TRUE(near(inverse(float4x4{1.f}), float4x4{1.f}));
}

TEST(Float4x4, InverseRoundTrip) {
    // M * inverse(M) == I
    float4x4 m = translate(float4x4{1.f}, {3, -1, 2});
    float4x4 mi = inverse(m);
    float4x4 should_be_id = m * mi;
    EXPECT_TRUE(near(should_be_id, float4x4{1.f}, 1e-4f));
}

TEST(Float4x4, RowMajorLayout) {
    // Memory layout: row0.xyzw, row1.xyzw, row2.xyzw, row3.xyzw
    float4x4 m{
        {1, 2, 3, 4},    // row 0
        {5, 6, 7, 8},    // row 1
        {9, 10, 11, 12},
        {13, 14, 15, 16},
    };
    const float* p = value_ptr(m);
    // p[0..3] = row 0 = {1,2,3,4}
    EXPECT_EQ(p[0], 1.f);
    EXPECT_EQ(p[1], 2.f);
    EXPECT_EQ(p[2], 3.f);
    EXPECT_EQ(p[3], 4.f);
    // p[4..7] = row 1 = {5,6,7,8}
    EXPECT_EQ(p[4], 5.f);
    EXPECT_EQ(p[5], 6.f);
}

// ── Functions ─────────────────────────────────────────────────────────────────

TEST(Functions, DotProduct) {
    float3 a{1, 0, 0}, b{0, 1, 0};
    EXPECT_NEAR(dot(a, b), 0.f, kEps);
    EXPECT_NEAR(dot(a, a), 1.f, kEps);
}

TEST(Functions, CrossProduct) {
    float3 x{1, 0, 0}, y{0, 1, 0};
    EXPECT_EQ(cross(x, y), (float3{0, 0, 1}));
    EXPECT_EQ(cross(y, x), (float3{0, 0, -1}));
}

TEST(Functions, NormalizeUnit) {
    float3 v{3, 0, 0};
    float3 n = normalize(v);
    EXPECT_TRUE(near(n, {1, 0, 0}));
    EXPECT_NEAR(length(n), 1.f, kEps);
}

TEST(Functions, InverseTranspose) {
    // inverseTranspose(I) == I
    float3x3 id = float3x3::identity();
    EXPECT_EQ(inverseTranspose(id), id);
}

TEST(Functions, Radians) {
    EXPECT_NEAR(radians(180.f), pi<float>(), kEps);
    EXPECT_NEAR(degrees(pi<float>()), 180.f, kEps);
}

TEST(Functions, Mat3FromMat4) {
    float4x4 m{1.f};
    m[3] = {99, 99, 99, 1}; // row 3 (homogeneous row) — should be ignored in 3×3 extraction
    float3x3 m3 = mat3(m);
    EXPECT_EQ(m3, float3x3::identity());
}

// ── Quaternion ────────────────────────────────────────────────────────────────

TEST(Quaternion, IdentityToMatrix) {
    float4x4 m = toMatrix(quaternion::identity());
    EXPECT_TRUE(near(m, float4x4{1.f}));
}

TEST(Quaternion, AngleAxisY90) {
    // 90-degree rotation about Y: x→z, z→-x
    const float angle = radians(90.f);
    quaternion q = angleAxis(angle, {0, 1, 0});
    float4x4   m = mat4_cast(q);
    float4 x_col{1, 0, 0, 0};
    float4 result = m * x_col;
    EXPECT_NEAR(result.x,  0.f, 1e-4f);
    EXPECT_NEAR(result.z, -1.f, 1e-4f); // RH: rotate X 90° about Y gives -Z
}

// ── Transform builders ────────────────────────────────────────────────────────

TEST(Transform, TranslateRoundTrip) {
    float4x4 t = translate(float4x4{1.f}, {5, -3, 2});
    float4 p{0, 0, 0, 1};
    float4 tp = t * p;
    EXPECT_NEAR(tp.x, 5.f, kEps);
    EXPECT_NEAR(tp.y, -3.f, kEps);
    EXPECT_NEAR(tp.z, 2.f, kEps);
}

TEST(Transform, ScaleVector) {
    float4x4 s = scale(float4x4{1.f}, {2, 3, 4});
    float4 v{1, 1, 1, 0};
    float4 sv = s * v;
    EXPECT_NEAR(sv.x, 2.f, kEps);
    EXPECT_NEAR(sv.y, 3.f, kEps);
    EXPECT_NEAR(sv.z, 4.f, kEps);
}

TEST(Transform, LookAtBuildsOrthonormalBasis) {
    float4x4 v = lookAtRH({0, 0, 5}, {0, 0, 0}, {0, 1, 0});
    // With row-major storage, rows 0-2 contain the orthonormal camera basis vectors.
    float3 row0{v[0][0], v[0][1], v[0][2]};  // right
    float3 row1{v[1][0], v[1][1], v[1][2]};  // up
    float3 row2{v[2][0], v[2][1], v[2][2]};  // -forward
    EXPECT_NEAR(length(row0), 1.f, kEps);
    EXPECT_NEAR(length(row1), 1.f, kEps);
    EXPECT_NEAR(length(row2), 1.f, kEps);
    EXPECT_NEAR(dot(row0, row1), 0.f, kEps);
    EXPECT_NEAR(dot(row0, row2), 0.f, kEps);
}

TEST(Transform, PerspectiveDepthRange) {
    // Near plane should map to depth 0, far to depth 1 (Vulkan ZO convention).
    float4x4 p = perspectiveRH_ZO(radians(90.f), 1.f, 0.1f, 100.f);
    // A point at z = -zNear (camera-space): NDC depth should be 0.
    float4 near_pt{0, 0, -0.1f, 1};
    float4 ndc = p * near_pt;
    EXPECT_NEAR(ndc.z / ndc.w, 0.f, 1e-4f);
    // A point at z = -zFar: NDC depth should be 1.
    float4 far_pt{0, 0, -100.f, 1};
    float4 ndc_far = p * far_pt;
    EXPECT_NEAR(ndc_far.z / ndc_far.w, 1.f, 1e-4f);
}

TEST(Transform, RowVectorShaderConvention) {
    // Verify mul(v, M) == M^T * v (shader convention test).
    // Build a simple non-identity matrix.
    float4x4 m = translate(float4x4{1.f}, {1, 2, 3});
    float4   v{4, 5, 6, 1};
    float4 row_result = v * m;
    float4 col_result = transpose(m) * v;
    EXPECT_TRUE(near(row_result, col_result));
}
