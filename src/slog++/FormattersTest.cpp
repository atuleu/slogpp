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
		EXPECT_EQ(slog::details::ToString(data.Duration), data.Expected);
	}
}

TEST(Formatters, JSON) {
	struct TestData {
		std::string                 Name;
		std::string                 Expected;
		std::shared_ptr<RecordBase> Input;
	};

	TimeT ts{};
	ts += std::chrono::hours(24);

	std::vector<TestData> testdata = {
	    {
	        "Simple",
	        "{\"time\":86400.0,\"level\":\"INFO\","
	        "\"message\":\"simple logging\"}",
	        std::make_unique<Record<0>>(ts, Level::Info, "simple logging"),
	    },
	    {"WithFields",
	     "{\"time\":86400.0,\"level\":\"INFO\","
	     "\"message\":\"simple attribute "
	     "logging\",\"anInt\":1,\"aDouble\":1.5,\"aString\":\"hello "
	     "world\",\"aTimeStamp\":\"1970-01-02T00:00:00.000Z\"}",
	     std::make_shared<Record<4>>(
	         ts,
	         Level::Info,
	         "simple attribute logging",
	         Int("anInt", 1),
	         Float("aDouble", 1.5),
	         String("aString", "hello world"),
	         Time("aTimestamp", TimeT{})
	     )},
	};

	for (const auto &data : testdata) {
		SCOPED_TRACE(data.Name);
		std::string buffer;
		EXPECT_NO_THROW(RecordToJSON(*data.Input, buffer));
		EXPECT_EQ(buffer, data.Expected);
	}
}

}; // namespace slog
