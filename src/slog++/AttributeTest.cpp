#include "AttributeTest.hpp"

#include "Attribute.hpp"
#include <chrono>
#include <gtest/gtest.h>
#include <variant>

namespace slog {

template <typename T> void testInteger(const std::string &key, T &&value) {
	SCOPED_TRACE(key);
	auto a = Int(key, std::forward<T>(value));

	EXPECT_EQ(a.key, key);
	EXPECT_NO_THROW({ EXPECT_EQ(std::get<int64_t>(a.value), value); });
	EXPECT_THROW(std::get<bool>(a.value), std::bad_variant_access);
}

TEST_F(AttributeTest, Int) {
	testInteger<int8_t>("int8", -3);
	testInteger<uint8_t>("uint8", 128);
	testInteger<int16_t>("int16", -23);
	testInteger<uint16_t>("uint16", 40000);
	testInteger<int32_t>("int32", -3000);
	testInteger<uint32_t>("uint32", 1234414);
	testInteger<int64_t>("int64", -12347461784);
	testInteger<uint64_t>("uint64", 12380);
}

template <typename T> void testFloating(const std::string &key, T &&value) {
	SCOPED_TRACE(key);
	auto a = Float(key, value);

	EXPECT_EQ(a.key, key);
	EXPECT_NO_THROW({ EXPECT_EQ(std::get<double>(a.value), value); });
	EXPECT_THROW(std::get<bool>(a.value), std::bad_variant_access);
}

TEST_F(AttributeTest, Float) {
	testFloating<float>("float", 12.1);
	testFloating<double>("double", 23.1);
}

TEST_F(AttributeTest, Bool) {
	auto a = Bool("bool", true);
	EXPECT_EQ(a.key, "bool");
	EXPECT_NO_THROW(EXPECT_EQ(std::get<bool>(a.value), true));
	EXPECT_THROW(std::get<double>(a.value), std::bad_variant_access);
}

template <typename T> void testString(const std::string &key, T &&value) {
	SCOPED_TRACE(key);
	auto a = String(key, value);

	EXPECT_EQ(a.key, key);
	EXPECT_NO_THROW({ EXPECT_EQ(std::get<std::string>(a.value), value); });
	EXPECT_THROW(std::get<bool>(a.value), std::bad_variant_access);
}

TEST_F(AttributeTest, String) {
	testString<const char *>("char*", "Hello");
	testString<std::string>("std::string", std::string("World"));
}

TEST_F(AttributeTest, DurationT) {
	auto a = Duration("duration", std::chrono::hours(1));
	EXPECT_EQ(a.key, "duration");
	EXPECT_NO_THROW(
	    EXPECT_EQ(std::get<DurationT>(a.value), std::chrono::hours(1))
	);
}

TEST_F(AttributeTest, TimeT) {
	auto epoch            = std::chrono::system_clock::time_point{};
	auto oneDayAfterEpoch = epoch + std::chrono::hours(24);

	auto a = Time("time", epoch + std::chrono::hours(24));
	auto b = Time("now", std::chrono::system_clock::now());

	EXPECT_EQ(a.key, "time");
	EXPECT_NO_THROW({
		auto time = std::get<TimeT>(a.value);
		EXPECT_EQ(
		    time,
		    std::chrono::time_point_cast<DurationT>(oneDayAfterEpoch)
		);
		EXPECT_EQ((time - epoch).count(), 24 * 3600 * 1e9);
	});
}

TEST_F(AttributeTest, Group) {
	auto a = Group(
	    "group",
	    String("request", "https://example.com"),
	    Int("status", 200)
	);
	EXPECT_EQ(a.key, "group");
	EXPECT_NO_THROW({
		const auto &group = *std::get<GroupPtr>(a.value);
		ASSERT_EQ(group.attributes.size(), 2);
		EXPECT_EQ(group.attributes[0].key, "request");
		EXPECT_EQ(
		    std::get<std::string>(group.attributes[0].value),
		    "https://example.com"
		);
		EXPECT_EQ(group.attributes[1].key, "status");
		EXPECT_EQ(std::get<int64_t>(group.attributes[1].value), 200);
	});
}

TEST_F(AttributeTest, Error) {
	EXPECT_EQ(Error("oops"), (Attribute{"error", "oops"}));
	EXPECT_EQ(
	    Error(std::runtime_error("something went wrong")),
	    (Attribute{"error", "something went wrong"})
	);
}

TEST_F(AttributeTest, ByReference) {
	int         a;
	float       b;
	std::string c;
	DurationT   d;
	TimeT       e;

	Attribute aa = Int("a", a);
	Attribute bb = Float("b", b);
	Attribute cc = String("c", c);
	Attribute dd = Duration("d", d);
	Attribute ee = Time("e", e);
	Group("f", aa, bb, cc, dd, ee);
}

} // namespace slog
