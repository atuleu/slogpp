#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <type_traits>
#include <variant>
#include <vector>

namespace slog {
struct Attribute;

typedef std::shared_ptr<std::vector<Attribute>> GroupPtr;

using Value = std::variant<bool, int64_t, double, std::string, GroupPtr>;

struct Attribute {
	std::string key;
	Value       value;
};

template <typename Str> Attribute Bool(const Str &key, bool value);

template <typename Str, typename Integer,
          std::enable_if_t<std::is_integral_v<Integer>, int> = 0>
Attribute Int(const Str &key, Integer value);

template <typename Str, typename Floating,
          std::enable_if_t<std::is_floating_point_v<Floating>, int> = 0>
Attribute Float(const Str &key, Floating value);

template <
    typename Str, typename T,
    std::enable_if_t<std::is_convertible_v<const T &, std::string>, int> = 0>
Attribute String(const Str &key, const T &value);

template <typename Str, typename... Attributes>
Attribute Group(const Str &key, Attributes &&...attributes);

} // namespace slog

#include "AttributeImpl.hpp"
