if(NOT CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
	# externally linked via FetchContent. Simply create slog++::slog++ interface
	# target
	add_library(slog++::slog++ INTERFACE IMPORTED GLOBAL)
	target_include_directories(slog++::slog++ INTERFACE include)

	return()
endif(NOT CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)

cmake_minimum_required(VERSION 3.16)
cmake_policy(SET CMP0048 NEW)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

include(GitVersion)

git_version(PROJECT_PREFIX slogpp_ #
			FALLBACK_VERSION 0.0.0
)

project(
	slogpp #
	LANGUAGES CXX #
	VERSION ${PROJECT_VERSION} #
)

message(STATUS "Building slogpp ${slogpp_FULL_VERSION}")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED On)

include(FetchContent)
FetchContent_Declare(
	googletest
	GIT_REPOSITORY https://github.com/google/googletest.git
	GIT_TAG main
)

set(gtest_force_shared_crt
	On
	CACHE BOOL "" FORCE
)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_custom_target(check ALL COMMAND ${CMAKE_CTEST_COMMAND} ARGS
									--output-on-failure
)

include(GoogleTest)
include_directories(include)
add_subdirectory(include/slog++)

option(SLOG_BENCHMARKS Off)

if(SLOG_BENCHMARKS)
	add_subdirectory(benchmarks)
endif(SLOG_BENCHMARKS)
