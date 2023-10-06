#include "AttributeTest.hpp"

#include "Attribute.hpp"
#include <gtest/gtest.h>
#include <variant>

namespace slog {

template <typename T> void testInteger(const std::string &key, T &&value) {
	SCOPED_TRACE(key);
	auto a = Int(key, value);

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

TEST_F(AttributeTest, Group) {
	auto a = Group("group", String("request", "https://example.com"),
	               Int("status", 200));
	EXPECT_EQ(a.key, "group");
	EXPECT_NO_THROW({
		const std::vector<Attribute> &group = *std::get<GroupPtr>(a.value);
		ASSERT_EQ(group.size(), 2);
		EXPECT_EQ(group[0].key, "request");
		EXPECT_EQ(std::get<std::string>(group[0].value), "https://example.com");
		EXPECT_EQ(group[1].key, "status");
		EXPECT_EQ(std::get<int64_t>(group[1].value), 200);
	});
}

} // namespace slog
