#include <cmath>
#include <cstdint>
#include <gtest/gtest.h>
#include <limits>
#include <slang-math/slang-math.hpp>

using namespace sm;

// ── Helpers ──────────────────────────────────────────────────────────────────

static constexpr float kEps = 1e-5f;

static bool near(float a, float b, float eps = kEps) {
    return std::abs(a - b) <= eps;
}
static bool near(const float2& a, const float2& b, float eps = kEps) {
    return near(a.x, b.x, eps) && near(a.y, b.y, eps);
}
static bool near(const float3& a, const float3& b, float eps = kEps) {
    return near(a.x, b.x, eps) && near(a.y, b.y, eps) && near(a.z, b.z, eps);
}
static bool near(const float4& a, const float4& b, float eps = kEps) {
    return near(a.x, b.x, eps) && near(a.y, b.y, eps) && near(a.z, b.z, eps) && near(a.w, b.w, eps);
}
static bool near(const float4x4& a, const float4x4& b, float eps = kEps) {
    for (std::int32_t c = 0; c < 4; ++c)
        if (!near(a[c], b[c], eps))
            return false;
    return true;
}
static bool near(const float2x2& a, const float2x2& b, float eps = kEps) {
    for (std::int32_t r = 0; r < 2; ++r)
        if (!near(a[r], b[r], eps))
            return false;
    return true;
}
static bool near(const float3x3& a, const float3x3& b, float eps = kEps) {
    for (std::int32_t r = 0; r < 3; ++r)
        if (!near(a[r], b[r], eps))
            return false;
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
    EXPECT_EQ(-a, (float2{-1, -2}));
}

TEST(Float2, Index) {
    float2 v{5, 7};
    EXPECT_EQ(v[0], 5.f);
    EXPECT_EQ(v[1], 7.f);
}

TEST(Float2, RgAlias) {
    float2 c{0.1f, 0.2f};
    EXPECT_EQ(c.r, c.x);
    EXPECT_EQ(c.g, c.y);
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
    EXPECT_EQ(-a, (float3{-1, -2, -3}));
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
    auto back = static_cast<float3>(w);
    EXPECT_EQ(back, v);
}

// ── float4x4 ─────────────────────────────────────────────────────────────────

TEST(Float4x4, IdentityDiagonal) {
    float4x4 m{1.f};
    for (std::int32_t row = 0; row < 4; ++row)
        for (std::int32_t col = 0; col < 4; ++col)
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
    for (std::int32_t r = 0; r < 4; ++r)
        for (std::int32_t c = 0; c < 4; ++c)
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
        {1, 2, 3, 4}, // row 0
        {5, 6, 7, 8}, // row 1
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

TEST(Functions, SaturateClampsToUnitRange) {
    // scalar + vector: below 0 -> 0, above 1 -> 1, in-range unchanged
    EXPECT_EQ(saturate(-0.25f), 0.0f);
    EXPECT_EQ(saturate(0.5f), 0.5f);
    EXPECT_EQ(saturate(1.75f), 1.0f);
    EXPECT_EQ((saturate(float2{-1, 2})), (float2{0, 1}));
    EXPECT_EQ((saturate(float3{-3, 0.5f, 3})), (float3{0, 0.5f, 1}));
    EXPECT_EQ((saturate(float4{-1, 0, 1, 5})), (float4{0, 0, 1, 1}));
    // endpoints are exact: saturate is the SM6 gate for shader migrations
    EXPECT_EQ(saturate(0.0f), 0.0f);
    EXPECT_EQ(saturate(1.0f), 1.0f);
}

TEST(Functions, Mat3FromMat4) {
    float4x4 m{1.f};
    m[3] = {99, 99, 99, 1}; // row 3 (homogeneous row) — should be ignored in 3×3 extraction
    float3x3 m3 = toFloat3x3(m);
    EXPECT_EQ(m3, float3x3::identity());
}

// ── Quaternion ────────────────────────────────────────────────────────────────

TEST(Quaternion, IdentityToMatrix) {
    float4x4 m = toFloat4x4(quaternion::identity());
    EXPECT_TRUE(near(m, float4x4{1.f}));
}

TEST(Quaternion, AngleAxisY90) {
    // 90-degree rotation about Y: x→z, z→-x
    const float angle = radians(90.f);
    quaternion q = angleAxis(angle, {0, 1, 0});
    float4x4 m = toFloat4x4(q);
    float4 x_col{1, 0, 0, 0};
    float4 result = m * x_col;
    EXPECT_NEAR(result.x, 0.f, 1e-4f);
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
    float4x4 v = lookAt({0, 0, 5}, {0, 0, 0}, {0, 1, 0});
    // With row-major storage, rows 0-2 contain the orthonormal camera basis vectors.
    float3 row0{v[0][0], v[0][1], v[0][2]}; // right
    float3 row1{v[1][0], v[1][1], v[1][2]}; // up
    float3 row2{v[2][0], v[2][1], v[2][2]}; // -forward
    EXPECT_NEAR(length(row0), 1.f, kEps);
    EXPECT_NEAR(length(row1), 1.f, kEps);
    EXPECT_NEAR(length(row2), 1.f, kEps);
    EXPECT_NEAR(dot(row0, row1), 0.f, kEps);
    EXPECT_NEAR(dot(row0, row2), 0.f, kEps);
}

TEST(Transform, PerspectiveDepthRange) {
    // Near plane should map to depth 0, far to depth 1 (Vulkan ZO convention).
    float4x4 p = perspective(radians(90.f), 1.f, 0.1f, 100.f);
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
    float4 v{4, 5, 6, 1};
    float4 row_result = v * m;
    float4 col_result = transpose(m) * v;
    EXPECT_TRUE(near(row_result, col_result));
}

// ── SM4: TRS compose / ortho / inverseOrtho ──

TEST(Transform, TrsComposeMatchesChainedBuilders) {
    const float3 t{1.f, -2.f, 3.f};
    const quaternion q = angleAxis(radians(30.f), normalize(float3{1.f, 1.f, 1.f}));
    const float3 s{2.f, -1.5f, 0.5f};
    const float4x4 composed = trs(t, q, s);
    // T * R * S via the existing post-multiply builders:
    // translate first, then the quaternion rotation, then scale.
    const float4x4 chained = translate(identity<float4x4>(), t) * toFloat4x4(q) * scale(identity<float4x4>(), s);
    EXPECT_TRUE(near(composed, chained));
}

TEST(Transform, TrsMapsOriginAndUnitAxes) {
    const float3 t{4.f, -1.f, 7.f};
    const quaternion q = angleAxis(radians(90.f), float3{0.f, 1.f, 0.f});
    const float3 s{2.f, 3.f, 4.f};
    const float4x4 m = trs(t, q, s);
    const float4 origin{0.f, 0.f, 0.f, 1.f};
    const float4 op = m * origin;
    EXPECT_NEAR(op.x, t.x, kEps);
    EXPECT_NEAR(op.y, t.y, kEps);
    EXPECT_NEAR(op.z, t.z, kEps);
    // Unit X scaled by 2, rotated 90 deg about Y -> (0, 0, -2) + t.
    const float4 xAxis = m * float4{1.f, 0.f, 0.f, 1.f};
    EXPECT_NEAR(xAxis.x, t.x, kEps);
    EXPECT_NEAR(xAxis.z, t.z - s.x, kEps);
}

TEST(Transform, TrsIdentityIsIdentity) {
    EXPECT_TRUE(near(trs({0, 0, 0}, identity<quaternion>(), {1, 1, 1}), identity<float4x4>()));
}

TEST(Transform, OrthoDepthRange) {
    // Near plane maps to depth 0, far plane to depth 1 (Vulkan ZO convention).
    const float4x4 o = ortho(-10.f, 10.f, -5.f, 5.f, 0.1f, 100.f);
    const float4 near_pt{0.f, 0.f, -0.1f, 1.f};
    const float4 ndc = o * near_pt;
    EXPECT_NEAR(ndc.z / ndc.w, 0.f, 1e-4f);
    const float4 far_pt{0.f, 0.f, -100.f, 1.f};
    const float4 ndc_far = o * far_pt;
    EXPECT_NEAR(ndc_far.z / ndc_far.w, 1.f, 1e-4f);
}

TEST(Transform, OrthoMapsBoxExtentsToNdcExtents) {
    const float4x4 o = ortho(-10.f, 10.f, -5.f, 5.f, 0.1f, 100.f);
    const float4 right_top_far{10.f, 5.f, -100.f, 1.f};
    const float4 ndc = o * right_top_far;
    EXPECT_NEAR(ndc.x / ndc.w, 1.f, 1e-4f);
    EXPECT_NEAR(ndc.y / ndc.w, 1.f, 1e-4f);
    EXPECT_NEAR(ndc.z / ndc.w, 1.f, 1e-4f);
    const float4 left_bottom_near{-10.f, -5.f, -0.1f, 1.f};
    const float4 ndc2 = o * left_bottom_near;
    EXPECT_NEAR(ndc2.x / ndc2.w, -1.f, 1e-4f);
    EXPECT_NEAR(ndc2.y / ndc2.w, -1.f, 1e-4f);
    EXPECT_NEAR(ndc2.z / ndc2.w, 0.f, 1e-4f);
}

TEST(Transform, InverseOrthoRoundTrip) {
    const float4x4 o = ortho(-10.f, 10.f, -5.f, 5.f, 0.1f, 100.f);
    const float4x4 oi = inverseOrtho(-10.f, 10.f, -5.f, 5.f, 0.1f, 100.f);
    EXPECT_TRUE(near(o * oi, identity<float4x4>()));
    EXPECT_TRUE(near(oi * o, identity<float4x4>()));
    const float4 pt{3.f, -2.f, -42.f, 1.f};
    EXPECT_TRUE(near(oi * (o * pt), pt));
}

// ── Inverse projection / view (S2: back the shipped sky reprojection fix) ──────

TEST(InverseProjection, PerspectiveInverseRoundTrip) {
    // inversePerspective must be the exact inverse of perspective.
    const float4x4 p = perspective(radians(60.f), 16.f / 9.f, 0.1f, 250.f);
    const float4x4 pInv = inversePerspective(radians(60.f), 16.f / 9.f, 0.1f, 250.f);
    EXPECT_TRUE(near(p * pInv, float4x4{1.f}, 1e-4f));
    EXPECT_TRUE(near(pInv * p, float4x4{1.f}, 1e-4f));
}

TEST(InverseProjection, AnalyticalPerspectiveMatchesGenericInverse) {
    // The analytical inverse must agree with the generic Gauss-Jordan inverse.
    const float4x4 p = perspective(radians(45.f), 1.333f, 0.5f, 500.f);
    EXPECT_TRUE(near(inversePerspective(radians(45.f), 1.333f, 0.5f, 500.f), inverse(p), 1e-4f));
}

TEST(InverseProjection, LookAtInverseRoundTrip) {
    // inverseLookAt must be the exact inverse of lookAt.
    const float3 eye{3, 4, 10}, center{0, 1, 0}, up{0, 1, 0};
    const float4x4 v = lookAt(eye, center, up);
    const float4x4 vInv = inverseLookAt(eye, center, up);
    EXPECT_TRUE(near(v * vInv, float4x4{1.f}, 1e-4f));
    EXPECT_TRUE(near(vInv * v, float4x4{1.f}, 1e-4f));
}

TEST(InverseProjection, AnalyticalLookAtMatchesGenericInverse) {
    const float3 eye{-2, 5, 7}, center{1, 0, -1}, up{0, 1, 0};
    EXPECT_TRUE(near(inverseLookAt(eye, center, up), inverse(lookAt(eye, center, up)), 1e-4f));
}

TEST(InverseProjection, InvViewProjEqualsProductOfAnalyticalInverses) {
    // (P*V)^-1 == V^-1 * P^-1 — the identity that lets Application replace the
    // generic sm::inverse(curViewProj) with the cheap analytical pair.
    const float3 eye{0, 2, 6}, center{0, 0, 0}, up{0, 1, 0};
    const float4x4 view = lookAt(eye, center, up);
    const float4x4 proj = perspective(radians(50.f), 4.f / 3.f, 0.1f, 100.f);
    const float4x4 vp = proj * view;
    const float4x4 invVpGeneric = inverse(vp);
    const float4x4 invVpAnalytical =
        inverseLookAt(eye, center, up) * inversePerspective(radians(50.f), 4.f / 3.f, 0.1f, 100.f);
    EXPECT_TRUE(near(invVpGeneric, invVpAnalytical, 1e-3f));
    EXPECT_TRUE(near(vp * invVpGeneric, float4x4{1.f}, 1e-4f));
}

// Replicate the shader's background-reprojection math (motion_vector.comp.slang):
// reconstruct the world-space view ray for a pixel from invViewProj, then project
// that direction (point at infinity, w=0) through another frame's viewProj.
static float2 reprojectBackground(const float4x4& invCurViewProj, const float4x4& prevViewProj, float2 ndc) {
    const float4 farW = invCurViewProj * float4{ndc.x, ndc.y, 1.f, 1.f};
    const float4 nearW = invCurViewProj * float4{ndc.x, ndc.y, 0.f, 1.f};
    const float3 dir = static_cast<float3>(farW) / farW.w - static_cast<float3>(nearW) / nearW.w;
    const float4 prevClip = prevViewProj * float4{dir, 0.f};
    return {prevClip.x / prevClip.w, prevClip.y / prevClip.w};
}

TEST(SkyReprojection, StaticCameraGivesZeroMotion) {
    // With prev == cur, a background pixel must reproject to itself (motion ~ 0).
    const float4x4 vp = perspective(radians(60.f), 16.f / 9.f, 0.1f, 100.f) * lookAt({0, 0, 5}, {0, 0, 0}, {0, 1, 0});
    const float4x4 invVp = inverse(vp);
    for (float2 ndc : {float2{0, 0}, float2{0.5f, -0.3f}, float2{-0.8f, 0.6f}}) {
        const float2 prevNdc = reprojectBackground(invVp, vp, ndc);
        EXPECT_TRUE(near(prevNdc, ndc, 1e-3f));
    }
}

TEST(SkyReprojection, CameraRotationGivesFiniteNonZeroMotion) {
    // Rotating the camera between frames must yield a finite, non-zero background
    // motion vector (the sky tracks camera rotation instead of getting MV = 0).
    const float4x4 proj = perspective(radians(60.f), 16.f / 9.f, 0.1f, 100.f);
    const float4x4 curView = lookAt({0, 0, 5}, {0, 0, 0}, {0, 1, 0});
    const float4x4 prevView = lookAt({0, 0, 5}, {0.5f, 0, 0}, {0, 1, 0}); // yawed
    const float4x4 curVp = proj * curView;
    const float4x4 prevVp = proj * prevView;
    const float2 ndc{0.3f, 0.1f};
    const float2 prevNdc = reprojectBackground(inverse(curVp), prevVp, ndc);
    EXPECT_TRUE(std::isfinite(prevNdc.x) && std::isfinite(prevNdc.y));
    EXPECT_GT(length(prevNdc - ndc), 1e-3f); // motion is non-zero under rotation

    // Self-consistency: projecting the reconstructed ray through the CURRENT VP
    // must return the original pixel (proves invCurViewProj is a true inverse).
    const float2 selfNdc = reprojectBackground(inverse(curVp), curVp, ndc);
    EXPECT_TRUE(near(selfNdc, ndc, 1e-3f));
}

// ── Quaternion algebra ────────────────────────────────────────────────────────

TEST(QuaternionAlgebra, NormalizeProducesUnitLength) {
    quaternion q = normalize(quaternion{1, 2, 3, 4});
    EXPECT_NEAR(std::sqrt(dot(q, q)), 1.f, kEps);
}

TEST(QuaternionAlgebra, ConjugateInvertsRotation) {
    // q * conjugate(q) == identity for a unit quaternion.
    const quaternion q = angleAxis(radians(37.f), {0.3f, 0.7f, -0.2f});
    const quaternion qc = q * conjugate(q);
    EXPECT_TRUE(near(float3{qc.x, qc.y, qc.z}, {0, 0, 0}, 1e-5f));
    EXPECT_NEAR(qc.w, 1.f, 1e-5f);
}

TEST(QuaternionAlgebra, MultiplyComposesRotations) {
    // (a * b) applied to v == a applied to (b applied to v).
    const quaternion a = angleAxis(radians(90.f), {0, 1, 0});
    const quaternion b = angleAxis(radians(90.f), {1, 0, 0});
    const float3 v{0, 0, 1};
    const float3 composed = (a * b) * v;
    const float3 sequential = a * (b * v);
    EXPECT_TRUE(near(composed, sequential, 1e-4f));
}

TEST(QuaternionAlgebra, MultiplyMatchesMatrixComposition) {
    // toFloat4x4(a*b) == toFloat4x4(a) * toFloat4x4(b).
    const quaternion a = angleAxis(radians(40.f), {0, 1, 0});
    const quaternion b = angleAxis(radians(25.f), {1, 0, 0});
    EXPECT_TRUE(near(toFloat4x4(a * b), toFloat4x4(a) * toFloat4x4(b), 1e-4f));
}

TEST(QuaternionAlgebra, SlerpEndpoints) {
    const quaternion a = angleAxis(radians(0.f), {0, 1, 0});
    const quaternion b = angleAxis(radians(90.f), {0, 1, 0});
    EXPECT_TRUE(near(slerp(a, b, 0.f) * float3{1, 0, 0}, a * float3{1, 0, 0}, 1e-4f));
    EXPECT_TRUE(near(slerp(a, b, 1.f) * float3{1, 0, 0}, b * float3{1, 0, 0}, 1e-4f));
}

TEST(QuaternionAlgebra, SlerpMidpointIsHalfAngle) {
    // Halfway between 0° and 90° about Y is a 45° rotation.
    const quaternion a = angleAxis(radians(0.f), {0, 1, 0});
    const quaternion b = angleAxis(radians(90.f), {0, 1, 0});
    const quaternion mid = slerp(a, b, 0.5f);
    const quaternion expected = angleAxis(radians(45.f), {0, 1, 0});
    EXPECT_TRUE(near(mid * float3{1, 0, 0}, expected * float3{1, 0, 0}, 1e-4f));
}

// ── Generic vector functions — float2 / float4 now at parity with float3 ──────

TEST(Float2Functions, DotLengthNormalize) {
    float2 a{3, 4};
    EXPECT_NEAR(dot(a, a), 25.f, kEps);
    EXPECT_NEAR(length(a), 5.f, kEps);
    EXPECT_TRUE(near(normalize(a), float2{0.6f, 0.8f}));
}

TEST(Float4Functions, DotLengthNormalize) {
    float4 a{0, 0, 3, 4};
    EXPECT_NEAR(dot(a, a), 25.f, kEps);
    EXPECT_NEAR(length(a), 5.f, kEps);
    EXPECT_TRUE(near(normalize(a), float4{0, 0, 0.6f, 0.8f}));
}

TEST(Float2Functions, MinMaxClamp) {
    EXPECT_EQ((min(float2{1, 8}, float2{5, 2})), (float2{1, 2}));
    EXPECT_EQ((max(float2{1, 8}, float2{5, 2})), (float2{5, 8}));
    EXPECT_EQ((clamp(float2{-1, 9}, 0.f, 5.f)), (float2{0, 5}));
    EXPECT_EQ((clamp(float2{-1, 9}, float2{0, 0}, float2{5, 5})), (float2{0, 5}));
}

TEST(Float4Functions, MinMaxClamp) {
    EXPECT_EQ((min(float4{1, 8, 3, 0}, float4{5, 2, 3, 9})), (float4{1, 2, 3, 0}));
    EXPECT_EQ((clamp(float4{-1, 9, 4, -2}, 0.f, 5.f)), (float4{0, 5, 4, 0}));
}

TEST(Float2Functions, LerpAbsSqrtPow) {
    EXPECT_TRUE(near(lerp(float2{0, 0}, float2{10, 20}, 0.5f), float2{5, 10}));
    EXPECT_TRUE(near(lerp(float2{0, 0}, float2{10, 20}, float2{1, 0}), float2{10, 0}));
    EXPECT_EQ((abs(float2{-2, 3})), (float2{2, 3}));
    EXPECT_TRUE(near(sqrt(float2{4, 9}), float2{2, 3}));
    EXPECT_TRUE(near(pow(float2{2, 3}, 2.f), float2{4, 9}));
}

TEST(Float4Functions, UnaryAndReflectDistance) {
    // Reflect [1,0,0,0] about [1,0,0,0] -> [-1,0,0,0]; dot with forward -> -1.
    EXPECT_NEAR(dot(reflect(float4{1, 0, 0, 0}, float4{1, 0, 0, 0}), float4{1, 0, 0, 0}), -1.f, kEps);
    EXPECT_NEAR(distance(float4{0, 0, 0, 0}, float4{0, 3, 4, 0}), 5.f, kEps);
    EXPECT_NEAR(cos(float4{0, 0, 0, 0}).x, 1.f, kEps);
    EXPECT_NEAR(exp(float4{0, 0, 0, 0}).y, 1.f, kEps);
    EXPECT_NEAR(log(float4{1, 1, 1, 1}).z, 0.f, kEps);
}

TEST(Float2Functions, DegenerateNormalizeIsZero) {
    EXPECT_EQ(normalize(float2{0, 0}), (float2{0, 0}));
}

// ── uint3 + uint4 arithmetic ──────────────────────────────────────────────────

TEST(Uint3, BasicArithmetic) {
    uint3 a{1, 2, 3}, b{4, 5, 6};
    EXPECT_EQ(a + b, (uint3{5, 7, 9}));
    EXPECT_EQ(b - a, (uint3{3, 3, 3}));
    EXPECT_EQ(a * 2u, (uint3{2, 4, 6}));
    EXPECT_EQ(a[2], 3u);
}

TEST(Uint4, BasicArithmetic) {
    uint4 a{1, 2, 3, 4}, b{10, 20, 30, 40};
    EXPECT_EQ(a + b, (uint4{11, 22, 33, 44}));
    EXPECT_EQ(b - a, (uint4{9, 18, 27, 36}));
    EXPECT_EQ(a * b, (uint4{10, 40, 90, 160}));
    EXPECT_EQ(a * 2u, (uint4{2, 4, 6, 8}));
}

// ── float2x2 ──────────────────────────────────────────────────────────────────

TEST(Float2x2, Identity) {
    float2x2 id{1.f};
    for (std::int32_t r = 0; r < 2; ++r)
        for (std::int32_t c = 0; c < 2; ++c)
            EXPECT_EQ(id[r][c], (r == c) ? 1.f : 0.f);
    EXPECT_EQ(identity<float2x2>(), id);
}

TEST(Float2x2, MatrixVectorMultiply) {
    float2x2 m{{2, 0}, {0, 3}};
    EXPECT_EQ((m * float2{1, 1}), (float2{2, 3}));
}

TEST(Float2x2, Transpose) {
    float2x2 m{{1, 2}, {3, 4}};
    EXPECT_EQ(transpose(m), (float2x2{{1, 3}, {2, 4}}));
}

TEST(Float2x2, MatrixMultiply) {
    float2x2 a{{1, 2}, {3, 4}};
    float2x2 b{{5, 6}, {7, 8}};
    EXPECT_EQ(a * b, (float2x2{{19, 22}, {43, 50}}));
    EXPECT_EQ(a * float2x2{1.f}, a);
    EXPECT_EQ(float2x2{1.f} * a, a);
}

TEST(Float2x2, Determinant) {
    EXPECT_NEAR(determinant(float2x2{{1, 2}, {3, 4}}), -2.f, kEps);
    EXPECT_NEAR(determinant(float2x2{1.f}), 1.f, kEps);
}

TEST(Float2x2, InverseRoundTrip) {
    float2x2 m{{4, 7}, {2, 6}};
    float2x2 mi = inverse(m);
    EXPECT_TRUE(near(m * mi, float2x2{1.f}, 1e-4f));
    EXPECT_TRUE(near(mi * m, float2x2{1.f}, 1e-4f));
    EXPECT_EQ(inverse(float2x2{{1, 2}, {2, 4}}), float2x2{1.f}); // singular -> identity
}

TEST(Float2x2, InverseTransposeAndValuePtr) {
    EXPECT_EQ(inverseTranspose(float2x2{1.f}), float2x2{1.f});
    float2x2 m{{1, 2}, {3, 4}};
    const float* p = value_ptr(m);
    EXPECT_EQ(p[0], 1.f);
    EXPECT_EQ(p[1], 2.f);
    EXPECT_EQ(p[2], 3.f);
    EXPECT_EQ(p[3], 4.f);
}

TEST(Float2x2, ExtractFromLargerMatrices) {
    float3x3 m3{{1, 2, 9}, {3, 4, 9}, {9, 9, 5}};
    EXPECT_EQ(toFloat2x2(m3), (float2x2{{1, 2}, {3, 4}}));
    float4x4 m4{{1, 2, 9, 9}, {3, 4, 9, 9}, {9, 9, 9, 9}, {9, 9, 9, 9}};
    EXPECT_EQ(toFloat2x2(m4), (float2x2{{1, 2}, {3, 4}}));
}

// ── Generic matrix ops across sizes ───────────────────────────────────────────

TEST(GenericMatrix, TransposeRoundTripAllSizes) {
    float2x2 a{{1, 2}, {3, 4}};
    EXPECT_EQ(transpose(transpose(a)), a);
    float3x3 b{{1, 2, 3}, {4, 5, 6}, {7, 8, 9}};
    EXPECT_EQ(transpose(transpose(b)), b);
}

TEST(GenericMatrix, Float3x3InverseAndInverseTranspose) {
    float3x3 m{{4, 3, 0}, {1, 1, 0}, {0, 0, 5}};
    EXPECT_TRUE(near(inverse(m) * m, float3x3{1.f}, 1e-4f));
    EXPECT_TRUE(near(inverseTranspose(float3x3{1.f}), float3x3{1.f}));
}

// ── Concepts (compile-time contract) ──────────────────────────────────────────

TEST(Concepts, Satisfaction) {
    static_assert(vec<float2> && vec<float3> && vec<float4>);
    static_assert(vec<uint2> && vec<uint3> && vec<uint4>);
    static_assert(float_vec<float2> && float_vec<float3> && float_vec<float4>);
    static_assert(!float_vec<uint2> && !float_vec<uint3> && !float_vec<uint4>);
    static_assert(square_mat<float2x2> && square_mat<float3x3> && square_mat<float4x4>);
    static_assert(!vec<float2x2> && !vec<float4x4>);
    static_assert(!vec<quaternion> && !float_vec<quaternion>);
    SUCCEED();
}

// ── uint vectors — element-generic ops (min/max/clamp/dot/value_ptr) ──────────

TEST(UintFunctions, MinMaxClampDotValuePtr) {
    EXPECT_EQ((min(uint3{1, 8, 3}, uint3{5, 2, 9})), (uint3{1, 2, 3}));
    EXPECT_EQ((max(uint3{1, 8, 3}, uint3{5, 2, 9})), (uint3{5, 8, 9}));
    EXPECT_EQ((clamp(uint3{0, 9, 4}, 1u, 5u)), (uint3{1, 5, 4}));
    EXPECT_EQ(dot(uint3{1, 2, 3}, uint3{4, 5, 6}), 1u * 4 + 2u * 5 + 3u * 6);
    uint3 v{7, 8, 9};
    const std::uint32_t* p = value_ptr(v);
    EXPECT_EQ(p[0], 7u);
    EXPECT_EQ(p[2], 9u);
}

// ── Common functions (SM1) ───────────────────────────────────────────────────

TEST(CommonFunctions, SignScalarAndVector) {
    EXPECT_EQ(sign(-3.5f), -1.f);
    EXPECT_EQ(sign(3.5f), 1.f);
    EXPECT_EQ(sign(0.f), 0.f);
    EXPECT_EQ(sign(-0.f), 0.f);
    EXPECT_EQ(sign(std::numeric_limits<float>::quiet_NaN()), 0.f);
    EXPECT_TRUE(near(sign(float3{-3.f, 0.f, 2.f}), float3{-1.f, 0.f, 1.f}));
}

TEST(CommonFunctions, FloorCeilTruncRoundVector) {
    EXPECT_TRUE(near(floor(float2{2.7f, -2.1f}), float2{2.f, -3.f}));
    EXPECT_TRUE(near(ceil(float2{2.1f, -2.7f}), float2{3.f, -2.f}));
    EXPECT_TRUE(near(trunc(float3{2.7f, -2.7f, 0.9f}), float3{2.f, -2.f, 0.f}));
    EXPECT_TRUE(near(round(float2{2.4f, 2.5f}), float2{2.f, 3.f}));
    EXPECT_TRUE(near(round(float2{2.5f, -2.5f}), float2{3.f, -3.f})); // half away from zero
    EXPECT_TRUE(near(round(float4{2.6f, -2.4f, 0.5f, -0.5f}), float4{3.f, -2.f, 1.f, -1.f}));
}

TEST(CommonFunctions, FracScalarAndVector) {
    EXPECT_FLOAT_EQ(frac(2.75f), 0.75f);
    EXPECT_FLOAT_EQ(frac(-1.25f), 0.75f);
    for (const float x : {-1.25f, 0.f, 0.5f, 3.99f, 7.f}) {
        const float f = frac(x);
        EXPECT_GE(f, 0.f);
        EXPECT_LT(f, 1.f);
        EXPECT_FLOAT_EQ(std::floor(x) + f, x); // x == floor(x) + frac(x)
    }
    EXPECT_TRUE(near(frac(float2{2.75f, -1.25f}), float2{0.75f, 0.75f}));
}

TEST(CommonFunctions, FmodVectorSignFollowsDividend) {
    EXPECT_TRUE(near(fmod(float2{5.5f, -5.5f}, float2{2.f, 2.f}), float2{1.5f, -1.5f}));
    EXPECT_TRUE(near(fmod(float2{5.5f, 5.5f}, float2{2.f, -2.f}), float2{1.5f, 1.5f}));
}

TEST(CommonFunctions, RsqrtScalarAndVector) {
    EXPECT_FLOAT_EQ(rsqrt(4.f), 0.5f);
    EXPECT_NEAR(rsqrt(2.f) * std::sqrt(2.f), 1.f, 1e-6f);
    EXPECT_TRUE(near(rsqrt(float3{1.f, 4.f, 0.25f}), float3{1.f, 0.5f, 2.f}));
    const float3 reciprocal = rsqrt(float3{2.f, 2.f, 2.f});
    EXPECT_NEAR(reciprocal.x * std::sqrt(2.f), 1.f, 1e-6f);
}

TEST(CommonFunctions, FmaVector) {
    EXPECT_TRUE(
        near(fma(float3{2.f, -2.f, 0.5f}, float3{3.f, 3.f, 4.f}, float3{1.f, 1.f, 1.f}), float3{7.f, -5.f, 3.f}));
}

TEST(CommonFunctions, StepScalarAndVector) {
    EXPECT_FLOAT_EQ(step(1.f, 0.5f), 0.f);
    EXPECT_FLOAT_EQ(step(1.f, 1.f), 1.f);
    EXPECT_FLOAT_EQ(step(1.f, 1.5f), 1.f);
    EXPECT_TRUE(near(step(float2{1.f, 2.f}, float2{0.5f, 2.5f}), float2{0.f, 1.f}));
}

TEST(CommonFunctions, Exp2Log2Log10Vector) {
    EXPECT_TRUE(near(exp2(float2{1.f, 3.f}), float2{2.f, 8.f}));
    EXPECT_TRUE(near(log2(float2{2.f, 8.f}), float2{1.f, 3.f}));
    EXPECT_TRUE(near(log10(float3{10.f, 100.f, 1000.f}), float3{1.f, 2.f, 3.f}));
    EXPECT_TRUE(near(log2(exp2(float2{7.5f, -2.f})), float2{7.5f, -2.f}));
    EXPECT_TRUE(near(exp2(log2(float3{5.f, 0.1f, 123.f})), float3{5.f, 0.1f, 123.f}));
}

TEST(CommonFunctions, PowVectorExponent) {
    const float3 base{2.f, 4.f, 8.f};
    const float3 exponent{2.f, 0.5f, 3.f};
    EXPECT_TRUE(near(pow(base, exponent), float3{4.f, 2.f, 512.f}));
    EXPECT_TRUE(near(pow(base, float3{2.f, 2.f, 2.f}), pow(base, 2.f))); // V,V vs V,float
}

TEST(CommonFunctions, ClassificationVector) {
    const float nan = std::numeric_limits<float>::quiet_NaN();
    const float inf = std::numeric_limits<float>::infinity();
    EXPECT_TRUE(any_nan(float4{1.f, nan, 2.f, 3.f})); // any component
    EXPECT_FALSE(any_nan(float4{1.f, 2.f, 3.f, 4.f}));
    EXPECT_TRUE(any_inf(float3{inf, 1.f, 2.f}));
    EXPECT_TRUE(any_inf(float3{1.f, -inf, 2.f}));
    EXPECT_FALSE(any_inf(float3{1.f, 2.f, 3.f}));
    EXPECT_TRUE(all_finite(float3{1.f, 2.f, 3.f}));
    EXPECT_FALSE(all_finite(float3{1.f, inf, 3.f}));
    EXPECT_FALSE(all_finite(float2{1.f, nan}));
}

// ── Trig + geometric (SM2) ───────────────────────────────────────────────────

TEST(TrigVector, SinTanAtanKnownValues) {
    const float p6 = pi<float>() / 6.f;
    const float p4 = pi<float>() / 4.f;
    const float3 angles{0.f, p6, p4};
    EXPECT_NEAR(sin(angles).x, 0.f, kEps);
    EXPECT_NEAR(sin(angles).y, 0.5f, kEps);
    EXPECT_NEAR(sin(angles).z, 0.7071068f, kEps);
    const float2 quarter{0.f, p4};
    EXPECT_NEAR(tan(quarter).x, 0.f, kEps);
    EXPECT_NEAR(tan(quarter).y, 1.f, kEps);
    const float2 unit{0.f, 1.f};
    EXPECT_NEAR(atan(unit).x, 0.f, kEps);
    EXPECT_NEAR(atan(unit).y, p4, kEps); // atan(1) == pi/4
    const float4 zeros{0.f, 0.f, 0.f, 0.f};
    EXPECT_NEAR(sin(zeros).w, 0.f, kEps);
    EXPECT_NEAR(tan(zeros).w, 0.f, kEps);
}

TEST(TrigVector, AsinAcosRoundTrips) {
    const float3 x{-1.2f, 0.4f, 1.5f}; // within [-pi/2, pi/2]
    EXPECT_TRUE(near(asin(sin(x)), x));
    const float3 y{0.3f, 1.5f, 3.0f}; // within [0, pi]
    EXPECT_TRUE(near(acos(cos(y)), y));
    const float2 ends{1.f, -1.f};
    EXPECT_NEAR(asin(ends).x, pi<float>() / 2.f, kEps);
    EXPECT_NEAR(asin(ends).y, -pi<float>() / 2.f, kEps);
    EXPECT_NEAR(acos(ends).x, 0.f, kEps);
    EXPECT_NEAR(acos(ends).y, pi<float>(), kEps);
}

TEST(TrigVector, Atan2Quadrants) {
    const float p4 = pi<float>() / 4.f;
    const float3 b{0.f, 1.f, 1.f};
    const float3 a{1.f, 1.f, -1.f};
    const float3 r = atan2(b, a);
    EXPECT_NEAR(r.x, 0.f, kEps);      // atan2(0, 1)
    EXPECT_NEAR(r.y, p4, kEps);       // atan2(1, 1)
    EXPECT_NEAR(r.z, 3.f * p4, kEps); // atan2(1, -1)
    const float2 b2{0.f, -1.f};
    const float2 a2{-1.f, 1.f};
    const float2 r2 = atan2(b2, a2);
    EXPECT_NEAR(r2.x, pi<float>(), kEps); // atan2(0, -1)
    EXPECT_NEAR(r2.y, -p4, kEps);         // atan2(-1, 1)
}

TEST(GeometryVector, FaceForward) {
    const float3 n{0.f, 1.f, 0.f};
    const float3 i{0.f, -1.f, 0.f}; // incident, pointing into the surface
    // nref already against i (dot(nref, i) = -1 < 0): n returned unchanged.
    EXPECT_TRUE(near(faceforward(n, i, float3{0.f, 1.f, 0.f}), n));
    // nref aligned with i (dot(nref, i) = +1 > 0): n flipped.
    EXPECT_TRUE(near(faceforward(n, i, float3{0.f, -1.f, 0.f}), -n));
    // float2 genericity: dot(nref, i) < 0 keeps n.
    EXPECT_EQ(faceforward(float2{3.f, 4.f}, float2{1.f, 0.f}, float2{-1.f, 0.f}), (float2{3.f, 4.f}));
}

TEST(GeometryVector, RefractNormalIncidenceAndTIR) {
    // Sign convention: i points INTO the surface, n points back against i.
    // Normal incidence with eta = 1: k = 1 -> refracted == incident (no deflection).
    EXPECT_TRUE(near(refract(float3{0.f, 0.f, -1.f}, float3{0.f, 0.f, 1.f}, 1.f), (float3{0.f, 0.f, -1.f})));
    // Dense -> rare beyond the critical angle: 60 deg with eta = 1.5 gives
    // k = 1 - 1.5^2 * (1 - cos^2(60 deg)) = -0.6875 < 0 -> total internal reflection -> zero vector.
    const float3 grazing{std::sin(radians(60.f)), 0.f, -std::cos(radians(60.f))};
    EXPECT_EQ(refract(grazing, float3{0.f, 0.f, 1.f}, 1.5f), (float3{0.f, 0.f, 0.f}));
}

TEST(GeometryVector, RefractGlass45Degrees) {
    // Air -> glass, 45 deg incidence (eta = 1/1.5): sin(theta_t) = sin(45 deg) / 1.5.
    const float s45 = std::sin(radians(45.f));
    const float3 i{s45, 0.f, -s45}; // unit incident, 45 deg from the +z normal
    const float3 t = refract(i, float3{0.f, 0.f, 1.f}, 1.f / 1.5f);
    const float sT = s45 / 1.5f;
    const float cT = std::sqrt(1.f - sT * sT);
    EXPECT_TRUE(near(t, (float3{sT, 0.f, -cT}), 1e-5f));
    EXPECT_NEAR(length(t), 1.f, 1e-5f); // transmitted ray stays unit
}

TEST(SmoothstepVector, EdgesMidpointMonotonicScalarConsistency) {
    const float3 e0{0.f, -1.f, 2.f};
    const float3 e1{1.f, 1.f, 4.f};
    const float3 belowMid = smoothstep(e0, e1, float3{-2.f, -1.f, 3.f});
    EXPECT_NEAR(belowMid.x, 0.f, kEps);  // below edge0 clamps to 0
    EXPECT_NEAR(belowMid.y, 0.f, kEps);  // at edge0
    EXPECT_NEAR(belowMid.z, 0.5f, kEps); // midpoint
    const float3 atAndAbove = smoothstep(e0, e1, float3{1.f, 5.f, 9.f});
    EXPECT_NEAR(atAndAbove.x, 1.f, kEps); // at edge1
    EXPECT_NEAR(atAndAbove.y, 1.f, kEps); // above edge1 clamps to 1
    EXPECT_NEAR(atAndAbove.z, 1.f, kEps);
    // Monotone non-decreasing in x.
    float prev = 0.f;
    for (std::int32_t s = 0; s <= 10; ++s) {
        const float u = static_cast<float>(s) / 10.f;
        const float v = smoothstep(float2{0.f, 0.f}, float2{1.f, 1.f}, float2{u, u}).x;
        EXPECT_GE(v, prev - 1e-6f);
        prev = v;
    }
    // Matches the scalar overload per component.
    const float3 xs{-0.25f, 0.4f, 1.7f};
    EXPECT_TRUE(near(smoothstep(float3{0.f, 0.f, 0.f}, float3{1.f, 1.f, 1.f}, xs),
                     (float3{smoothstep(0.f, 1.f, xs.x), smoothstep(0.f, 1.f, xs.y), smoothstep(0.f, 1.f, xs.z)})));
}

// ── Matrix/quaternion completeness (SM3) ─────────────────────────────────────

TEST(Float4x4, DeterminantKnownValues) {
    EXPECT_NEAR(determinant(float4x4{1.f}), 1.f, kEps); // det(I4) == 1
    // Triangular: product of the diagonal (2 * -1 * 3 * 0.5 == -3).
    const float4x4 triangular{{2.f, 1.f, 3.f, 0.f}, {0.f, -1.f, 4.f, 5.f}, {0.f, 0.f, 3.f, 2.f}, {0.f, 0.f, 0.f, 0.5f}};
    EXPECT_NEAR(determinant(triangular), -3.f, kEps);
    // Hand-computed cofactor expansion along the first row (all four 3x3 minors are +-38).
    const float4x4 m{{2.f, 1.f, 0.f, 2.f}, {4.f, 1.f, 5.f, 0.f}, {0.f, 2.f, 1.f, 1.f}, {6.f, 0.f, 2.f, 4.f}};
    EXPECT_NEAR(determinant(m), -38.f, 1e-4f);
    // Singular: row 1 is row 0 scaled by 2 -> det == 0.
    const float4x4 singular{{1.f, 2.f, 3.f, 4.f}, {2.f, 4.f, 6.f, 8.f}, {0.f, 1.f, 0.f, 0.f}, {1.f, 1.f, 1.f, 1.f}};
    EXPECT_NEAR(determinant(singular), 0.f, 1e-4f);
}

TEST(Float4x4, DeterminantMultiplicative) {
    const float4x4 a{{2.f, 1.f, 3.f, 0.f}, {0.f, -1.f, 4.f, 5.f}, {0.f, 0.f, 3.f, 2.f}, {0.f, 0.f, 0.f, 0.5f}};
    const float4x4 b{{2.f, 1.f, 0.f, 2.f}, {4.f, 1.f, 5.f, 0.f}, {0.f, 2.f, 1.f, 1.f}, {6.f, 0.f, 2.f, 4.f}};
    EXPECT_NEAR(determinant(a * b), determinant(a) * determinant(b), 1e-3f); // det(AB) == det(A) * det(B)
}

TEST(Float2x2, RowVectorMultiply) {
    const float2x2 m{{1.f, 2.f}, {3.f, 4.f}};
    // (v * M)[j] = sum_i v[i] * M[i][j]: (5*1 + 6*3, 5*2 + 6*4).
    EXPECT_TRUE(near(float2{5.f, 6.f} * m, float2{23.f, 34.f}));
    // Row-vector form differs from the column-vector member form: M * v == (17, 39).
    EXPECT_TRUE(near(m * float2{5.f, 6.f}, float2{17.f, 39.f}));
    // Identity: v * I == v.
    EXPECT_TRUE(near(float2{7.f, -3.f} * float2x2{1.f}, float2{7.f, -3.f}));
    // Transpose identity: v * M == transpose(M) * v.
    EXPECT_TRUE(near(float2{5.f, 6.f} * m, transpose(m) * float2{5.f, 6.f}));
}

TEST(Float3x3, RowVectorMultiply) {
    const float3x3 m{{1.f, 2.f, 3.f}, {4.f, 5.f, 6.f}, {7.f, 8.f, 10.f}};
    // (v * M)[j] = sum_i v[i] * M[i][j]: (30, 36, 45).
    EXPECT_TRUE(near(float3{1.f, 2.f, 3.f} * m, float3{30.f, 36.f, 45.f}));
    // Row-vector form differs from the column-vector member form: M * v == (14, 32, 53).
    EXPECT_TRUE(near(m * float3{1.f, 2.f, 3.f}, float3{14.f, 32.f, 53.f}));
    // Identity: v * I == v.
    EXPECT_TRUE(near(float3{7.f, -3.f, 2.f} * float3x3{1.f}, float3{7.f, -3.f, 2.f}));
    // Transpose identity: v * M == transpose(M) * v.
    EXPECT_TRUE(near(float3{1.f, 2.f, 3.f} * m, transpose(m) * float3{1.f, 2.f, 3.f}));
}

TEST(Quaternion, ToFloat3x3IdentityAndAxis90) {
    // Identity quaternion -> identity matrix.
    EXPECT_TRUE(near(toFloat3x3(quaternion::identity()), float3x3{1.f}));
    // The 90-degree axis rotations used by the toFloat4x4 tests, cropped to 3x3.
    const float s = std::sqrt(0.5f); // sin/cos of 45 degrees
    // 90 degrees about X: y -> z, z -> -y.
    const float3x3 rx{{1.f, 0.f, 0.f}, {0.f, 0.f, -1.f}, {0.f, 1.f, 0.f}};
    EXPECT_TRUE(near(toFloat3x3(quaternion{s, 0.f, 0.f, s}), rx, 1e-4f));
    // 90 degrees about Y: x -> -z, z -> x.
    const float3x3 ry{{0.f, 0.f, 1.f}, {0.f, 1.f, 0.f}, {-1.f, 0.f, 0.f}};
    EXPECT_TRUE(near(toFloat3x3(quaternion{0.f, s, 0.f, s}), ry, 1e-4f));
    // 90 degrees about Z: x -> y, y -> -x.
    const float3x3 rz{{0.f, -1.f, 0.f}, {1.f, 0.f, 0.f}, {0.f, 0.f, 1.f}};
    EXPECT_TRUE(near(toFloat3x3(quaternion{0.f, 0.f, s, s}), rz, 1e-4f));
    // Consistency with toFloat4x4: identical upper-left 3x3 block.
    const quaternion q = angleAxis(radians(37.f), normalize(float3{1.f, 2.f, -1.f}));
    EXPECT_TRUE(near(toFloat3x3(q), toFloat3x3(toFloat4x4(q))));
}

TEST(Quaternion, ToQuaternionRoundTripAllBranches) {
    // Round trip toFloat3x3(toQuaternion(m)) ~= m.  Identity and the 90-degree axis
    // rotations have trace > 0 (w branch); the 180-degree rotations drive the trace
    // negative and exercise the x / y / z diagonal branches.
    const float3x3 rx90{{1.f, 0.f, 0.f}, {0.f, 0.f, -1.f}, {0.f, 1.f, 0.f}};
    const float3x3 ry90{{0.f, 0.f, 1.f}, {0.f, 1.f, 0.f}, {-1.f, 0.f, 0.f}};
    const float3x3 rz90{{0.f, -1.f, 0.f}, {1.f, 0.f, 0.f}, {0.f, 0.f, 1.f}};
    const float3x3 rx180{{1.f, 0.f, 0.f}, {0.f, -1.f, 0.f}, {0.f, 0.f, -1.f}};
    const float3x3 ry180{{-1.f, 0.f, 0.f}, {0.f, 1.f, 0.f}, {0.f, 0.f, -1.f}};
    const float3x3 rz180{{-1.f, 0.f, 0.f}, {0.f, -1.f, 0.f}, {0.f, 0.f, 1.f}};
    const float3x3 rotations[] = {float3x3{1.f}, rx90, ry90, rz90, rx180, ry180, rz180};
    for (const float3x3& m : rotations)
        EXPECT_TRUE(near(toFloat3x3(toQuaternion(m)), m, 1e-4f));
    // A combined rotation built as a product of axis rotations round trips too.
    const float3x3 combined = rx90 * ry90 * rz180;
    EXPECT_TRUE(near(toFloat3x3(toQuaternion(combined)), combined, 1e-4f));
}

TEST(Quaternion, ToQuaternionKnownValue) {
    // 40 degrees about a normalized (1, 2, 3) axis: w = cos(20 deg) > 0.5, so
    // Shepperd's w branch recovers the exact quaternion (same sign).
    const quaternion q = angleAxis(radians(40.f), normalize(float3{1.f, 2.f, 3.f}));
    const quaternion back = toQuaternion(toFloat3x3(q));
    EXPECT_TRUE(near(float4{back.x, back.y, back.z, back.w}, float4{q.x, q.y, q.z, q.w}, 1e-4f));
    // The recovered quaternion is unit.
    EXPECT_NEAR(dot(back, back), 1.f, 1e-5f);
}
