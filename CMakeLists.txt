cmake_minimum_required(VERSION 3.16)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

include(GitVersion)

git_version(PROJECT_PREFIX cppslog_ #
			FALLBACK_VERSION 0.0.0
)

project(
	cppslog #
	LANGUAGES CXX #
	VERSION ${PROJECT_VERSION} #
)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED On)

add_subdirectory(include/slog++)

include_directories(include)

add_library(slog++ SHARED ${HDR_FILES} src/slog++.cpp)

include(FetchContent)
FetchContent_Declare(
	googletest
	URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)

set(gtest_force_shared_crt
	On
	CACHE BOOL "" FORCE
)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_subdirectory(tests)

add_executable(slog++-tests ${TEST_SRC_FILES} ${TEST_HDR_FILES})
target_link_libraries(slog++-tests GTest::gtest_main slog++)

include(GoogleTest)
gtest_discover_tests(slog++-tests)

add_custom_target(
	check
	COMMAND ${CMAKE_CTEST_COMMAND} ARGS --output-on-failure
	DEPENDS slog++-tests
)
