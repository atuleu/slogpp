#include "BenchmarkData.hpp"
#include "Benchmarker.hpp"
#include <chrono>
#include <thread>

void Noop(const BenchmarkData &) {}

int main() {
	Benchmarker<BenchmarkData, 100> benchmarker;

	auto noop = benchmarker.Benchmark("Noop", Noop);
	std::cout << noop << std::endl;
	return 0;
}
