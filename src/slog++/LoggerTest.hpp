#pragma once

#include <gtest/gtest.h>

#include "Logger.hpp"

#include "MockSink.hpp"

using ::testing::StrictMock;

namespace slog {
class LoggerTest : public ::testing::Test {
protected:
	std::unique_ptr<Logger<0>>            logger;
	std::shared_ptr<StrictMock<MockSink>> sink;

	void SetUp() override;
	void TearDown() override;
};

} // namespace slog
