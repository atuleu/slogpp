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
