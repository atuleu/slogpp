#include "Formatters.hpp"

#include <gtest/gtest.h>

namespace slog {

TEST(Formatters, JSON) {
	struct TestData {
		std::string Name;
		std::string Expected;
		Record      Input;
	};

	TimeT ts{};
	ts += std::chrono::hours(24);

	std::vector<TestData> testdata = {
	    {
	        "Simple",
	        "{\"time\":86400.0,\"level\":\"INFO\","
	        "\"message\":\"simple logging\"}",
	        Record(ts, Level::Info, "simple logging"),
	    },
	    {
	        "WithFields",
	        "{\"time\":86400.0,\"level\":\"INFO\","
	        "\"message\":\"simple "
	        "logging\",\"anInt\":1,\"aDouble\":1.5,\"aString\":\"hello "
	        "world\",\"aTimeStamp\":\"1970-01-02T00:00:00.000Z\"}",
	        Record(ts, Level::Info, "simple logging"),
	    },

	};

	for (const auto &data : testdata) {
		SCOPED_TRACE(data.Name);
		std::string buffer;
		EXPECT_NO_THROW(RecordToJSON(data.Input, buffer));
		EXPECT_EQ(buffer, data.Expected);
	}
}

}; // namespace slog
