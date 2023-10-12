#include "gtest/gtest.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <slog++/slog++.hpp>

namespace slog {

const static std::string rfc3339 =
    "[0-9]{4}-[0-9]{2}-[0-9]{2}T[0-9]{2}:[0-9]{2}:[0-9]{2}.[0-9]{3,9}Z";

// R"--(\d{4}-\d{2}-\d{2}T\d{2}:\d{2}:\d{2}.\d{3,9}Z)--";

TEST(SlogDefaultLogger, SingleLine) {
	::testing::internal::CaptureStderr();
	slog::Info("hello world", slog::Int("a", 23));
	auto res = ::testing::internal::GetCapturedStderr();
	EXPECT_THAT(
	    res,
	    ::testing::MatchesRegex(
	        "^" + rfc3339 + R"--( INFO "hello world" a=23)--" + "\n$"
	    )
	);
}

TEST(SlogDefaultLogger, MultiLine) {
	::testing::internal::CaptureStderr();
	auto logger = slog::With(String("domain", "coucou"));
	logger.Warn("ouch");
	logger.Info("ok");
	auto res = ::testing::internal::GetCapturedStderr();
	EXPECT_THAT(
	    res,
	    ::testing::MatchesRegex(
	        "^" + rfc3339 + " WARN ouch domain=coucou\n" + rfc3339 +
	        " INFO ok domain=coucou\n$"
	    )
	);
}

} // namespace slog
