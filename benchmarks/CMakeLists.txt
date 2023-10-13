set(HDR_FILES
	BenchmarkData.hpp #
	Types.hpp #
	Benchmarker.hpp #
)

set(SRC_FILES
	BenchmarkData.cpp #
	Benchmarker.cpp #
	main.cpp #
)

add_executable(slog++-benchmarks ${SRC_FILES} ${HDR_FILES})

add_custom_target(benchmarks
				  COMMAND ${CMAKE_CURRENT_BINARY_DIR}/slog++-benchmarks
)
