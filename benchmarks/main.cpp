#include "BenchmarkData.hpp"
#include "Benchmarker.hpp"
#include <chrono>
#include <thread>

void f(const BenchmarkData &) {
	std::this_thread::sleep_for(std::chrono::milliseconds(20));
}

int main() {
	Benchmarker<BenchmarkData, 100> benchmarker;

	benchmarker.Benchmark("sleeper", f);

	return 0;
}
