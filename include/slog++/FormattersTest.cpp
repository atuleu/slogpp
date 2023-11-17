#include "Formatters.hpp"

#include <chrono>
#include <gtest/gtest.h>
#include <sstream>

namespace slog {

TEST(Formatters, DurationT) {
	struct TestData {
		DurationT   Duration;
		std::string Expected;
	};

	using namespace std::chrono;
	std::vector<TestData> testdata = {
	    {nanoseconds(0), "0s"},
	    {nanoseconds(1), "1ns"},
	    {nanoseconds(1100), "1.1µs"},
	    {microseconds(2200), "2.2ms"},
	    {milliseconds(3300), "3.3s"},
	    {milliseconds(-4400), "-4.4s"},
	    {minutes(4) + seconds(5), "4m5s"},
	    {minutes(4) + seconds(5) + milliseconds(1), "4m5.001s"},
	    {hours(5) + minutes(6) + milliseconds(7001), "5h6m7.001s"},
	    {minutes(8) + nanoseconds(1), "8m1e-09s"},
	    {nanoseconds(std::numeric_limits<int64_t>::max()),
	     "2562047h47m16.854775807s"},
	    {nanoseconds(std::numeric_limits<int64_t>::min()),
	     "-2562047h47m16.854775808s"},
	};

	for (const auto &data : testdata) {
		std::string result;
		EXPECT_NO_THROW(slog::details::FormatTo(data.Duration, result));
		EXPECT_EQ(result, data.Expected);
	}
}

TEST(Formatters, TimeT) {
	struct TestData {
		TimeT       Time;
		std::string Expected;
	};

	using namespace std::chrono;
	std::vector<TestData> testdata = {
	    {TimeT{} + hours(48), "1970-01-03T00:00:00.000Z"},
	    {TimeT{} + hours(24) + nanoseconds(1),
	     "1970-01-02T00:00:00.000000001Z"},
	    {TimeT{} + milliseconds(999) + nanoseconds(30),
	     "1970-01-01T00:00:00.999000030Z"},
	    {TimeT{} + microseconds(999), "1970-01-01T00:00:00.000999Z"},
	    {TimeT{} + milliseconds(123), "1970-01-01T00:00:00.123Z"},
	}; // namespace slog

	for (const auto &data : testdata) {
		std::string result;
		EXPECT_NO_THROW(slog::details::FormatTo(data.Time, result));
		EXPECT_EQ(result, data.Expected);
	}
}

TEST(Formatters, JSON) {
	struct TestData {
		std::string             Name;
		std::string             Expected;
		std::shared_ptr<Record> Input;
	};

	TimeT ts{};
	ts += std::chrono::hours(24);

	std::vector<TestData> testdata = {
	    {
	        "Simple",
	        R"--({"time":"1970-01-02T00:00:00.000Z","level":"INFO","message":"simple logging"})--",
	        std::make_unique<details::Record<0>>(
	            ts,
	            Level::Info,
	            "simple logging"
	        ),
	    },
	    {
	        "WithFields",
	        R"--({"time":"1970-01-02T00:00:00.000Z","level":"INFO","message":"simple attribute logging","anInt":1,"aDouble":1.5,"aString":"hello world","aDuration":"32µs","aTimestamp":"1970-01-01T00:00:00.000Z"})--",
	        std::make_shared<details::Record<5>>(
	            ts,
	            Level::Info,
	            "simple attribute logging",
	            Int("anInt", 1),
	            Float("aDouble", 1.5),
	            String("aString", "hello world"),
	            Duration("aDuration", std::chrono::microseconds(32)),
	            Time("aTimestamp", TimeT{})
	        ),
	    },
	    {
	        "WithGroup",
	        R"--({"time":"1970-01-02T00:00:00.000Z","level":"INFO","message":"grouped attribute logging","aGroup":{"request":"https://example.com/","status":200}})--",
	        std::make_shared<details::Record<1>>(
	            ts,
	            Level::Info,
	            "grouped attribute logging",
	            Group(
	                "aGroup",
	                String("request", "https://example.com/"),
	                Int("status", 200)
	            )
	        ),
	    },
	    {
	        .Name = "ASCII",
	        .Expected =
	            R"--({"time":"1970-01-02T00:00:00.000Z","level":"INFO","message":"Hello\nWorld\t!"})--",
	        .Input = std::make_shared<details::Record<0>>(
	            ts,
	            Level::Info,
	            "Hello\nWorld\t!"
	        ),
	    },
	};

	for (const auto &data : testdata) {
		SCOPED_TRACE(data.Name);
		std::string buffer;
		EXPECT_NO_THROW(RecordToJSON(*data.Input, buffer));
		EXPECT_EQ(buffer, data.Expected);
	}
}

TEST(Formatters, RawText) {
	struct TestData {
		std::string             Name;
		std::string             Expected;
		std::shared_ptr<Record> Input;
	};

	TimeT ts{};
	ts += std::chrono::hours(24);

	std::vector<TestData> testdata = {
	    {
	        "Simple",
	        "1970-01-02T00:00:00.000Z WARN simple",
	        std::make_unique<details::Record<0>>(ts, Level::Warn, "simple"),
	    },
	    {
	        "WithFields",
	        R"--(1970-01-02T00:00:00.000Z ERROR "simple attribute logging" anInt=1 aDouble=1.5 aString="hello world" aDuration=32µs aTimestamp=1970-01-01T00:00:00.000Z)--",
	        std::make_shared<details::Record<5>>(
	            ts,
	            Level::Error,
	            "simple attribute logging",
	            Int("anInt", 1),
	            Float("aDouble", 1.5),
	            String("aString", "hello world"),
	            Duration("aDuration", std::chrono::microseconds(32)),
	            Time("aTimestamp", TimeT{})
	        ),
	    },
	    {
	        "WithGroup",
	        R"--(1970-01-02T00:00:00.000Z DEBUG "grouped attribute logging" request.URL=https://example.com/ request.status=200)--",
	        std::make_shared<details::Record<1>>(
	            ts,
	            Level::Debug,
	            "grouped attribute logging",
	            Group(
	                "request",
	                String("URL", "https://example.com/"),
	                Int("status", 200)
	            )
	        ),
	    },
	};

	for (const auto &data : testdata) {
		SCOPED_TRACE(data.Name);
		std::string buffer;
		EXPECT_NO_THROW(RecordToRawText(*data.Input, buffer));
		EXPECT_EQ(buffer, data.Expected);
	}
}

TEST(Formatters, ANSIText) {
	struct TestData {
		std::string             Name;
		std::string             Expected;
		std::shared_ptr<Record> Input;
	};

	TimeT ts{};
	ts += std::chrono::hours(24);

	std::vector<TestData> testdata = {
	    {
	        "Simple",
	        "1970-01-02T00:00:00.000Z \033[33mWARN\033[m simple",
	        std::make_unique<details::Record<0>>(ts, Level::Warn, "simple"),
	    },
	    {
	        "WithFields",
	        "1970-01-02T00:00:00.000Z "
	        "\033[31mERROR\033[m "
	        R"--("simple attribute logging"
├── anInt=1
├── aDouble=1.5
├── aString="hello world"
├── aDuration=32ms
└── aTimestamp=1970-01-01T00:00:00.000Z)--",
	        std::make_shared<details::Record<5>>(
	            ts,
	            Level::Error,
	            "simple attribute logging",
	            Int("anInt", 1),
	            Float("aDouble", 1.5),
	            String("aString", "hello world"),
	            Duration("aDuration", std::chrono::milliseconds(32)),
	            Time("aTimestamp", TimeT{})
	        ),
	    },
	    {
	        "WithGroup",
	        "1970-01-02T00:00:00.000Z "
	        "\033[34mDEBUG\033[m "
	        R"--("grouped attribute logging"
└── request
    ├── URL=https://example.com/
    └── status=200)--",
	        std::make_shared<details::Record<1>>(
	            ts,
	            Level::Debug,
	            "grouped attribute logging",
	            Group(
	                "request",
	                String("URL", "https://example.com/"),
	                Int("status", 200)
	            )
	        ),
	    },
	    {
	        "MultipleGroup",
	        "1970-01-02T00:00:00.000Z "
	        "TRACE_1\033[m "
	        R"--("multiple group attribute logging"
├── request
│   ├── URL=https://example.com/
│   └── status=200
└── meta
    ├── caching
    │   ├── cached=false
    │   └── processTime=12µs
    └── sizeKB=2345)--",
	        std::make_shared<details::Record<2>>(
	            ts,
	            SubLevel<Level::Trace, 1>,
	            "multiple group attribute logging",
	            Group(
	                "request",
	                String("URL", "https://example.com/"),
	                Int("status", 200)
	            ),
	            Group(
	                "meta",
	                Group(
	                    "caching",
	                    Bool("cached", false),
	                    Duration("processTime", std::chrono::microseconds(12))
	                ),
	                Int("sizeKB", 2345)
	            )
	        ),
	    },

	};

	for (const auto &data : testdata) {
		SCOPED_TRACE(data.Name);
		std::string buffer;
		EXPECT_NO_THROW(RecordToANSIText(*data.Input, buffer));
		EXPECT_EQ(buffer, data.Expected) << buffer;
	}
}

}; // namespace slog
