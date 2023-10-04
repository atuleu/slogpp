cmake_minimum_required(VERSION 3.16)
cmake_policy(SET CMP0048 NEW)

project(
  apollo
  LANGUAGES C CXX
  VERSION 0.0.1)

# GoogleTest requires at least C++14
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG main)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt
    ON
    CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)
enable_testing()

include(GoogleTest)

add_custom_target(check ALL ${CMAKE_CTEST_COMMAND} ARGS --output-on-failure)

find_package(glfw3 REQUIRED)
find_package(GLEW REQUIRED)
find_package(Eigen3 REQUIRED)

add_subdirectory(src/apollo)
