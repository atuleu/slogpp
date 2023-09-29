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

set(SRC_FILES src/slog++.cpp)

set(HDR_FILES include/slog++/Sink.hpp include/slog++/Logger.hpp)

add_library(slog++ SHARED ${SRC_FILES} ${HDR_FILES})
