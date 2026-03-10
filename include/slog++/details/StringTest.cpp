#include "String.hpp"
#include <gtest/gtest.h>

namespace slog {
namespace details {
class StringTest : public ::testing::Test {};

TEST_F(StringTest, DefaultIsEmpty) {
	auto empty = String{};
	EXPECT_EQ(empty.size(), 0);
	EXPECT_STREQ(empty.c_str(), "");
}

TEST_F(StringTest, SizeIsCorrect) {
	std::vector<std::string> testdata = {
	    "",
	    "a",
	    std::string(15, 'a'),
	    std::string(64, 'c'),
	    std::string(65, 'b')
	};
	for (const auto &s : testdata) {
		auto r = String(s);
		EXPECT_EQ(r.size(), s.size()) << "with input string '" << s << "'";
	}
}

TEST_F(StringTest, RepresentationIsCorrect) {
	std::vector<std::string> testdata =
	    {"", "a", std::string(15, 'a'), std::string(64, 'a')};
	for (const auto &s : testdata) {
		auto r = String(s.c_str());
		EXPECT_STREQ(r.c_str(), s.c_str()) << "with input string '" << s << "'";
		EXPECT_EQ(r.string_view(), s) << "with input string '" << s << "'";
	}
}

TEST_F(StringTest, LargeStringWithTrailingNullDoesNotKeepExactSize) {
	// due to the always odd-encoding of a size, a std::string containing
	// trailing null character will report the wrong size. The implementation
	// deliberatly do not test and avoid this condition. These objects are for
	// logging, and such strings contain no useful information for logging.
	std::string s(17, 'x'); // forces large representation (> 15)
	s[16] = '\0';           // last payload byte is null
	String r(s);
	// In that case, our string reports a different size.
	EXPECT_EQ(r.size(), s.size() - 1);
}

} // namespace details
} // namespace slog
