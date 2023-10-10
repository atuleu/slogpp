#include "LoggerTest.hpp"
#include "gmock/gmock.h"

namespace slog {

void LoggerTest::SetUp() {
	sink   = std::make_shared<StrictMock<MockSink>>();
	logger = std::unique_ptr<Logger<0>>(new Logger<0>(sink));
}

void LoggerTest::TearDown() {
	logger.reset();
	// will match expectation ??
	sink.reset();
}

using ::testing::_;
using ::testing::AllOf;
using ::testing::Contains;
using ::testing::Eq;
using ::testing::Field;
using ::testing::Gt;
using ::testing::InSequence;
using ::testing::Lt;
using ::testing::Return;
using ::testing::StrEq;

inline constexpr auto HasLevel = [](const auto &level) {
	return Field("level", &Record::level, Eq(level));
};

inline constexpr auto HasMessage = [](const auto &message) {
	return Field("message", &Record::message, StrEq(message));
};

inline constexpr auto HasAttribute = [](const auto &attribute) {
	return Field("attributes", &Record::attributes, Contains(attribute));
};

TEST_F(LoggerTest, LogHelperFunction) {

	struct TestData {
		Level                                    level;
		std::string                              message;
		std::function<void(const std::string &)> func;
	};

	std::vector<TestData> testdata = {
	    {Level::Trace,
	     "trace-message",
	     [this](const std::string &msg) { logger->Trace(msg); }},
	    {Level::Debug,
	     "debug-message",
	     [this](const std::string &msg) { logger->Debug(msg); }},
	    {Level::Info,
	     "info-message",
	     [this](const std::string &msg) { logger->Info(msg); }},
	    {Level::Warn,
	     "warn-message",
	     [this](const std::string &msg) { logger->Warn(msg); }},
	    {Level::Error,
	     "error-message",
	     [this](const std::string &msg) { logger->Error(msg); }},
	    {Level::Critical,
	     "critical-message",
	     [this](const std::string &msg) { logger->Critical(msg); }},
	};

	for (const auto &data : testdata) {
		SCOPED_TRACE(data.message);
		InSequence seq;
		EXPECT_CALL(*sink, Enabled(data.level)).WillOnce(Return(true));
		EXPECT_CALL(
		    *sink,
		    Log(AllOf(HasLevel(data.level), HasMessage(data.message)))
		);

		data.func(data.message);
	}
};

TEST_F(LoggerTest, LogEarlyDisabled) {

	{
		InSequence seq;
		EXPECT_CALL(*sink, Enabled(Level::Info)).WillOnce(Return(false));
		// No record produced, no more expectation.
		EXPECT_CALL(*sink, Log(_)).Times(0);
	}

	logger->Log(Level::Info, "never logged");
}

TEST_F(LoggerTest, AttributeLogging) {

	{
		InSequence seq;
		EXPECT_CALL(*sink, Enabled(Level::Info)).WillOnce(Return(true));
		EXPECT_CALL(
		    *sink,
		    Log(AllOf(
		        HasLevel(Level::Info),
		        HasMessage("with attribute"),
		        HasAttribute(Int("status", 200))
		    ))
		);
	}
	logger->Info("with attribute", Int("status", 200));
}

TEST_F(LoggerTest, AttributePropagation) {

	auto derived = logger->With(String("request", "https://example.com"));

	{
		InSequence seq;
		EXPECT_CALL(*sink, Enabled(Level::Warn)).WillOnce(Return(true));
		EXPECT_CALL(
		    *sink,
		    Log(AllOf(
		        HasLevel(Level::Warn),
		        HasMessage("unknown resource"),
		        HasAttribute(Int("status", 404)),
		        HasAttribute(String("request", "https://example.com"))
		    ))
		);
	}
	derived.Warn("unknown resource", Int("status", 404));
}

} // namespace slog
