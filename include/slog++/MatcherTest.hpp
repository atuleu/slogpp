#pragma once

#include <slog++/details/String.hpp>

#include "gmock/gmock.h"
#include <gmock/gmock.h>

#include "Record.hpp"

namespace slog {
using ::testing::ElementsAreArray;
using ::testing::Eq;
using ::testing::Field;
using ::testing::Pointee;
using ::testing::StrEq;
using ::testing::VariantWith;

template <typename T>
inline constexpr auto HasLevel = [](const auto &level) {
	return VariantWith<T>(Pointee(Field("level", &Record::level, Eq(level))));
};

template <typename T>
inline constexpr auto HasMessage = [](const auto &message) {
	return VariantWith<T>(
	    Pointee(Field("message", &Record::message, StrEq(message)))
	);
};

template <typename T, typename... Attributes>
auto HasAttributes(Attributes &&...attributes) {
	std::initializer_list<Attribute> attrs = {
	    static_cast<Attribute>(attributes)...,
	};
	return VariantWith<T>(Pointee(
	    Field("attributes", &Record::attributes, ElementsAreArray(attrs))
	));
}

} // namespace slog
