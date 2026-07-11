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
static bool near(const float2x2& a, const float2x2& b, float eps = kEps) {
    for (int r = 0; r < 2; ++r)
        if (!near(a[r], b[r], eps)) return false;
    return true;
}
static bool near(const float3x3& a, const float3x3& b, float eps = kEps) {
    for (int r = 0; r < 3; ++r)
        if (!near(a[r], b[r], eps)) return false;
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
    float4x4   m = toFloat4x4(q);
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
    float4x4 v = lookAt({0, 0, 5}, {0, 0, 0}, {0, 1, 0});
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
    float4   v{4, 5, 6, 1};
    float4 row_result = v * m;
    float4 col_result = transpose(m) * v;
    EXPECT_TRUE(near(row_result, col_result));
}

// ── Inverse projection / view (S2: back the shipped sky reprojection fix) ──────

TEST(InverseProjection, PerspectiveInverseRoundTrip) {
    // inversePerspective must be the exact inverse of perspective.
    const float4x4 p    = perspective(radians(60.f), 16.f / 9.f, 0.1f, 250.f);
    const float4x4 pInv = inversePerspective(radians(60.f), 16.f / 9.f, 0.1f, 250.f);
    EXPECT_TRUE(near(p * pInv, float4x4{1.f}, 1e-4f));
    EXPECT_TRUE(near(pInv * p, float4x4{1.f}, 1e-4f));
}

TEST(InverseProjection, AnalyticalPerspectiveMatchesGenericInverse) {
    // The analytical inverse must agree with the generic Gauss-Jordan inverse.
    const float4x4 p = perspective(radians(45.f), 1.333f, 0.5f, 500.f);
    EXPECT_TRUE(near(inversePerspective(radians(45.f), 1.333f, 0.5f, 500.f),
                     inverse(p), 1e-4f));
}

TEST(InverseProjection, LookAtInverseRoundTrip) {
    // inverseLookAt must be the exact inverse of lookAt.
    const float3 eye{3, 4, 10}, center{0, 1, 0}, up{0, 1, 0};
    const float4x4 v    = lookAt(eye, center, up);
    const float4x4 vInv = inverseLookAt(eye, center, up);
    EXPECT_TRUE(near(v * vInv, float4x4{1.f}, 1e-4f));
    EXPECT_TRUE(near(vInv * v, float4x4{1.f}, 1e-4f));
}

TEST(InverseProjection, AnalyticalLookAtMatchesGenericInverse) {
    const float3 eye{-2, 5, 7}, center{1, 0, -1}, up{0, 1, 0};
    EXPECT_TRUE(near(inverseLookAt(eye, center, up),
                     inverse(lookAt(eye, center, up)), 1e-4f));
}

TEST(InverseProjection, InvViewProjEqualsProductOfAnalyticalInverses) {
    // (P*V)^-1 == V^-1 * P^-1 — the identity that lets Application replace the
    // generic sm::inverse(curViewProj) with the cheap analytical pair.
    const float3 eye{0, 2, 6}, center{0, 0, 0}, up{0, 1, 0};
    const float4x4 view = lookAt(eye, center, up);
    const float4x4 proj = perspective(radians(50.f), 4.f / 3.f, 0.1f, 100.f);
    const float4x4 vp   = proj * view;
    const float4x4 invVpGeneric    = inverse(vp);
    const float4x4 invVpAnalytical = inverseLookAt(eye, center, up) *
                                     inversePerspective(radians(50.f), 4.f / 3.f, 0.1f, 100.f);
    EXPECT_TRUE(near(invVpGeneric, invVpAnalytical, 1e-3f));
    EXPECT_TRUE(near(vp * invVpGeneric, float4x4{1.f}, 1e-4f));
}

// Replicate the shader's background-reprojection math (motion_vector.comp.slang):
// reconstruct the world-space view ray for a pixel from invViewProj, then project
// that direction (point at infinity, w=0) through another frame's viewProj.
static float2 reprojectBackground(const float4x4& invCurViewProj,
                                  const float4x4& prevViewProj, float2 ndc) {
    const float4 farW  = invCurViewProj * float4{ndc.x, ndc.y, 1.f, 1.f};
    const float4 nearW = invCurViewProj * float4{ndc.x, ndc.y, 0.f, 1.f};
    const float3 dir   = static_cast<float3>(farW) / farW.w - static_cast<float3>(nearW) / nearW.w;
    const float4 prevClip = prevViewProj * float4{dir, 0.f};
    return {prevClip.x / prevClip.w, prevClip.y / prevClip.w};
}

TEST(SkyReprojection, StaticCameraGivesZeroMotion) {
    // With prev == cur, a background pixel must reproject to itself (motion ~ 0).
    const float4x4 vp = perspective(radians(60.f), 16.f / 9.f, 0.1f, 100.f) *
                        lookAt({0, 0, 5}, {0, 0, 0}, {0, 1, 0});
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
    const float4x4 curView  = lookAt({0, 0, 5}, {0, 0, 0}, {0, 1, 0});
    const float4x4 prevView = lookAt({0, 0, 5}, {0.5f, 0, 0}, {0, 1, 0}); // yawed
    const float4x4 curVp  = proj * curView;
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
    const quaternion a = angleAxis(radians(0.f),  {0, 1, 0});
    const quaternion b = angleAxis(radians(90.f), {0, 1, 0});
    EXPECT_TRUE(near(slerp(a, b, 0.f) * float3{1, 0, 0}, a * float3{1, 0, 0}, 1e-4f));
    EXPECT_TRUE(near(slerp(a, b, 1.f) * float3{1, 0, 0}, b * float3{1, 0, 0}, 1e-4f));
}

TEST(QuaternionAlgebra, SlerpMidpointIsHalfAngle) {
    // Halfway between 0° and 90° about Y is a 45° rotation.
    const quaternion a = angleAxis(radians(0.f),  {0, 1, 0});
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
    EXPECT_NEAR(dot(reflect(float4{1, 0, 0, 0}, float4{1, 0, 0, 0}),
                     float4{1, 0, 0, 0}), -1.f, kEps);
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
    for (int r = 0; r < 2; ++r)
        for (int c = 0; c < 2; ++c)
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
    static_assert(vec<uint2>  && vec<uint3>  && vec<uint4>);
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
    EXPECT_EQ(dot(uint3{1, 2, 3}, uint3{4, 5, 6}), 1u*4 + 2u*5 + 3u*6);
    uint3 v{7, 8, 9};
    const uint32_t* p = value_ptr(v);
    EXPECT_EQ(p[0], 7u);
    EXPECT_EQ(p[2], 9u);
}
