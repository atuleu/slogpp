set(HDR_FILES Benchmark.hpp)

set(SRC_FILES Benchmark.cpp)

add_executable(slog++-benchmarks ${SRC_FILES} ${HDR_FILES})

add_custom_target(benchmarks COMMAND ${CMAKE_CURRENT_BINARY_DIR}/slog++-benchmarks)
