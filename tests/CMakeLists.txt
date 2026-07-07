cmake_minimum_required(VERSION 3.25)
project(slang-math-tests CXX)

# Fetch GoogleTest
include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        v1.15.2
)
FetchContent_MakeAvailable(googletest)

add_executable(slang_math_tests test_slang_math.cpp)
target_link_libraries(slang_math_tests PRIVATE slang-math::slang-math GTest::gtest_main)
target_compile_options(slang_math_tests PRIVATE $<$<CXX_COMPILER_ID:Clang,AppleClang>:-Wno-language-extension-token>)

include(GoogleTest)
gtest_discover_tests(slang_math_tests)
