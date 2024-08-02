cmake_minimum_required(VERSION 3.14)
project(4value-vector)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

set(SOURCE_FILES
  vec4state.cpp
  vec4state.h
  vpi.h
)

add_executable(
  tests-vec4state
  tests-vec4state.cc
  ${SOURCE_FILES}
)
target_link_libraries(
  tests-vec4state
  GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(tests-vec4state)