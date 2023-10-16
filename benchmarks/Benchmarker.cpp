#include "Benchmarker.hpp"
#include "BenchmarkData.hpp"

BenchmarkResult::BenchmarkResult(std::vector<Duration> &&data)
    : durations{std::move(data)}
    , minimum{Duration::max()}
    , maximum{Duration::min()}
    , total{0}
    , ratio{1.0}
    , units{"ns"} {

	for (auto &d : durations) {
		total += d;
		if (d < minimum) {
			minimum = d;
		}
		if (d > maximum) {
			maximum = d;
		}
	}

	average      = total / durations.size();
}

std::tuple<double, std::string> Humanize(const Duration &d) noexcept {
	double res = d.count();
	if (res > 1e9) {
		return {res / 1e9, "s"};
	} else if (res > 1e6) {
		return {res / 1e6, "ms"};
	} else if (res > 1e3) {
		return {res / 1e3, "μs"};
	}
	return {res, "ns"};
}

std::ostream &operator<<(std::ostream &out, const BenchmarkResult &result) {
	auto format =
	    [&result](std::ostream &os, const Duration &d) -> std::ostream & {
		auto [v, units] = Humanize(d);
		return os << v << units;
	};

	out << result.durations.size() << " ops in ";
	format(out, result.total) << " avg: ";
	format(out, result.average) << " range:[";
	format(out, result.minimum) << ",";
	format(out, result.maximum) << "]";
	return out;
}
