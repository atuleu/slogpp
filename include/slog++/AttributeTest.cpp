#include "AttributeTest.hpp"

#include "Attribute.hpp"
#include "gmock/gmock.h"
#include <chrono>
#include <exception>
#include <gtest/gtest.h>
#include <variant>

namespace slog {

TEST_F(AttributeTest, Location) {
	auto loc = Location();
	EXPECT_EQ(loc.key, "location");
	EXPECT_NO_THROW({
		const auto &group = *std::get<GroupPtr>(loc.value);
		ASSERT_EQ(group.attributes.size(), 3);
		EXPECT_EQ(group.attributes[0].key, "function");
		EXPECT_STREQ(
		    std::get<StringType>(group.attributes[0].value).c_str(),
		    "virtual void slog::AttributeTest_Location_Test::TestBody()"
		);
		EXPECT_EQ(group.attributes[1].key, "file");
		EXPECT_THAT(
		    std::get<StringType>(group.attributes[1].value).c_str(),
		    ::testing::EndsWith("include/slog++/AttributeTest.cpp")
		);
		EXPECT_EQ(group.attributes[2].key, "line");
		EXPECT_EQ(std::get<long>(group.attributes[2].value), 13);
	});
}

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
	if constexpr (std::is_same_v<T, const char *>) {
		EXPECT_NO_THROW({
			EXPECT_STREQ(std::get<StringType>(a.value).c_str(), value);
		});
	} else if constexpr (std::is_same_v<T, std::string>) {
		EXPECT_NO_THROW({
			EXPECT_STREQ(std::get<StringType>(a.value).c_str(), value.c_str());
		});
	}
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

TEST_F(AttributeTest, Pointer) {
	auto pointer = std::make_unique<int>(42);
	auto p       = Pointer("pointer", pointer.get());
	EXPECT_EQ(p.key, "pointer");
	EXPECT_NO_THROW({
		EXPECT_EQ(std::get<void *>(p.value), (void *)pointer.get());
	});
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
		EXPECT_STREQ(
		    std::get<StringType>(group.attributes[0].value).c_str(),
		    "https://example.com"
		);
		EXPECT_EQ(group.attributes[1].key, "status");
		EXPECT_EQ(std::get<int64_t>(group.attributes[1].value), 200);
	});
}

class Exception : public std::exception {
public:
	const char *message() const noexcept {
		return "foo";
	}

	const char *what() const noexcept override {
		return "foo: bar";
	}
};

TEST_F(AttributeTest, Error) {
	EXPECT_EQ(Err("oops"), (Attribute{"error", "oops"}));
	EXPECT_EQ(
	    Err(std::runtime_error("something went wrong")),
	    (Attribute{"error", "something went wrong"})
	);
	EXPECT_EQ(Err(Exception{}), (Attribute{"error", "foo"}));
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
