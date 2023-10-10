#include "Formatters.hpp"

#include <gtest/gtest.h>

namespace slog {

TEST(Formatters, JSON) {
	struct TestData {
		std::string                 Name;
		std::string                 Expected;
		std::shared_ptr<RecordBase> Input;
	};

	TimeT ts{};
	ts += std::chrono::hours(24);

	std::vector<TestData> testdata = {
	    // // {
	    // //     "Simple",
	    // //     "{\"time\":86400.0,\"level\":\"INFO\","
	    // //     "\"message\":\"simple logging\"}",
	    // //     std::make_unique<Record<0>>(ts, Level::Info, "simple
	    // logging"),
	    // // },
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
