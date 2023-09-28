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

message(STATUS "Version is ${PROJECT_VERSION}")
