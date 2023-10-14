#include "BenchmarkData.hpp"
#include "Benchmarker.hpp"
#include <chrono>
#include <fstream>
#include <thread>

#include "Types.hpp"
#include "slog++/Config.hpp"
#include "slog++/slog++.hpp"
#include <glog/logging.h>

namespace pretty {
template <typename Clock, typename Duration>
std::ostream &operator<<(
    std::ostream &out, const std::chrono::time_point<Clock, Duration> &tp
) {

	// duplication of slog::details::FormatTo(const TimeT &, Buffer &);
	using namespace std::chrono;
	const auto timeS = time_point_cast<seconds>(tp);
	int64_t    nanos = (tp - timeS).count();
	const auto cTime = system_clock::to_time_t(timeS);

	if (nanos % 1000000 == 0) {
		nanos /= 1000000;
	} else if (nanos % 1000 == 0) {
		nanos /= 1000;
	}

	return out << std::put_time(gmtime(&cTime), "%FT%T") << "." << nanos << "Z";
}
}; // namespace pretty

struct NoopFunctor {
	void operator()(const BenchmarkData &) const {};
};

struct FstreamLogger {
	std::fstream stream;

	FstreamLogger(const std::string &filename)
	    : stream{filename, std::ios_base::out} {}

	void operator()(const BenchmarkData &data) {

		auto [value, units] = Humanize(data.duration);
		using pretty::operator<<;
		stream << Timepoint::clock::now() << " INFO "
		       << R"--( " new data " )--"
		       << "code=" << data.code                      //
		       << " value=" << data.value                   //
		       << " duration=" << value << units            //
		       << " time=" << data.time                     //
		       << " request.url=" << data.request.url       //
		       << " request.status=" << data.request.status //
		       << std::endl;
	}
};

struct SlogLogger {

	slog::Logger<0> logger;

	void operator()(const BenchmarkData &data) {
		using namespace slog;
		using slog::Duration;

		logger.Info(
		    "new data",
		    Int("code", data.code),
		    Float("value", data.value),
		    Duration("duration", data.duration),
		    Time("time", data.time),
		    Group(
		        "request",
		        String("url", data.request.url),
		        Int("status", data.request.status)
		    )
		);
	}

	template <typename... Options>
	SlogLogger(Options &&...options)
	    : logger{slog::BuildSink(std::forward<Options>(options)...)} {}
};

struct GLogLogger {
	GLogLogger(const char *argv0) {
		FLAGS_logtostderr = false;
		FLAGS_log_dir     = "/tmp";
		FLAGS_minloglevel = 0;
		google::InitGoogleLogging(argv0);
	}

	void operator()(const BenchmarkData &data) const {
		auto [value, units] = Humanize(data.duration);
		using pretty::operator<<;
		LOG(INFO) << Timepoint::clock::now() << " INFO "
		          << R"--( " new data " )--"
		          << "code=" << data.code                      //
		          << " value=" << data.value                   //
		          << " duration=" << value << units            //
		          << " time=" << data.time                     //
		          << " request.url=" << data.request.url       //
		          << " request.status=" << data.request.status //
		          << std::endl;
	}
};

int main(int argc, char **argv) {

	Benchmarker<BenchmarkData, 103> benchmarker;

	auto noop = benchmarker.Benchmark("Noop", NoopFunctor());
	std::cout << noop << std::endl;

	auto iostream =
	    benchmarker.Benchmark("Iostream - text", FstreamLogger("/dev/null"));
	std::cout << iostream << std::endl;

	auto slogText = benchmarker.Benchmark(
	    "slog++ - text",
	    SlogLogger(slog::WithFileOutput(
	        "/dev/null",
	        slog::WithFormat(slog::OutputFormat::TEXT),
	        slog::FromLevel(slog::Level::Info)
	    ))
	);
	std::cout << slogText << std::endl;

	auto slogJson = benchmarker.Benchmark(
	    "slog++ - JSON",
	    SlogLogger(slog::WithFileOutput(
	        "/dev/null",
	        slog::WithFormat(slog::OutputFormat::JSON),
	        slog::FromLevel(slog::Level::Info)
	    ))
	);
	std::cout << slogJson << std::endl;

	auto glogText =
	    benchmarker.Benchmark("Google GLog - Text", GLogLogger(argv[0]));
	std::cout << glogText << std::endl;

	return 0;
}
