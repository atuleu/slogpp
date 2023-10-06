#pragma once

#include "Attribute.hpp"
#include <initializer_list>

namespace slog {

template <typename Str> inline Attribute Bool(const Str &key, bool value) {
	return Attribute{key, value};
}

template <typename Str, typename Integer,
          std::enable_if_t<std::is_integral_v<Integer>, int>>
inline Attribute Int(const Str &key, Integer value) {
	return Attribute{key, int64_t(value)};
}

template <typename Str, typename Floating,
          std::enable_if_t<std::is_floating_point_v<Floating>, int>>
inline Attribute Float(const Str &key, Floating value) {
	return Attribute{key, double(value)};
}

template <typename Str, typename T,
          std::enable_if_t<std::is_convertible_v<const T &, std::string>, int>>
inline Attribute String(const Str &key, const T &value) {
	return Attribute{key, std::string(value)};
}

template <typename Str, typename... Attributes>
inline Attribute Group(const Str &key, Attributes &&...attributes) {
	auto group = std::make_shared<std::vector<Attribute>>(
	    std::initializer_list<Attribute>{
	        std::forward<Attributes>(attributes)...});

	return Attribute{key, group};
}

} // namespace slog
