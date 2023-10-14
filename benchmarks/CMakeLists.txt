set(HDR_FILES
	BenchmarkData.hpp #
	Types.hpp #
	Benchmarker.hpp #
	Ansi.hpp #
)

set(SRC_FILES
	BenchmarkData.cpp #
	Benchmarker.cpp #
	Ansi.cpp #
	main.cpp #
)

find_package(glog REQUIRED)

add_executable(slog++-benchmarks ${SRC_FILES} ${HDR_FILES})

target_link_libraries(slog++-benchmarks glog::glog)

add_custom_target(benchmarks
				  COMMAND ${CMAKE_CURRENT_BINARY_DIR}/slog++-benchmarks
)

add_dependencies(benchmarks slog++-benchmarks)
