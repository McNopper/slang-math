cmake_minimum_required(VERSION 3.25)
project(slang-math VERSION 0.1.5 LANGUAGES CXX)

# Header-only interface library — no compilation, no dependencies.
add_library(slang-math INTERFACE)
add_library(slang-math::slang-math ALIAS slang-math)

target_include_directories(slang-math INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
target_compile_features(slang-math INTERFACE cxx_std_20)

# Tests (opt-in via BUILD_TESTING or when building this as the top-level project)
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    include(CTest)
    enable_testing()
    add_subdirectory(tests)
endif()

