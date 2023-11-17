#pragma once

#include "Attribute.hpp"

#include <type_traits>

namespace slog {

template <typename Str> inline Attribute Bool(Str &&key, bool value) {
	return Attribute{std::forward<Str>(key), value};
}

template <
    typename Str,
    typename Integer,
    std::enable_if_t<std::is_integral_v<std::decay_t<Integer>>> *>
inline Attribute Int(Str &&key, Integer value) noexcept {
	return Attribute{std::forward<Str>(key), int64_t(value)};
}

template <
    typename Str,
    typename Floating,
    std::enable_if_t<std::is_floating_point_v<Floating>> *>
inline Attribute Float(Str &&key, Floating value) noexcept {
	return Attribute{std::forward<Str>(key), double(value)};
}

template <
    typename Str,
    typename T,
    std::enable_if_t<std::is_convertible_v<T, std::string>> *>
inline Attribute String(Str &&key, T &&value) noexcept {
	return Attribute{std::forward<Str>(key), std::forward<T>(value)};
}

template <
    typename Str,
    typename DurationType,
    std::enable_if_t<details::is_duration_castable<DurationType>::value> *>
inline Attribute Duration(Str &&key, DurationType &&value) noexcept {
	using namespace std::chrono;
	return Attribute{
	    std::forward<Str>(key),
	    duration_cast<DurationT>(std::forward<DurationType>(value)),
	};
}

template <
    typename Str,
    typename Timepoint,
    std::enable_if_t<details::is_time_castable<Timepoint>::value> *>
inline Attribute Time(Str &&key, Timepoint &&timepoint) noexcept {
	using namespace std::chrono;
	return Attribute{
	    std::forward<Str>(key),
	    time_point_cast<DurationT>(std::forward<Timepoint>(timepoint)),
	};
}

template <typename Str, typename... Attributes>
inline Attribute Group(Str &&key, Attributes &&...attributes) noexcept {
	static_assert(
	    sizeof...(Attributes) >= 1,
	    "A Group should always have at least one attribute"
	);
	using Array = details::GroupData<sizeof...(Attributes)>;
	return Attribute{
	    std::forward<Str>(key),
	    std::make_shared<Array>(std::forward<Attributes>(attributes)...),
	};
}

template <
    typename Str,
    std::enable_if_t<std::is_convertible_v<Str, std::string>, bool>>
inline Attribute Err(Str &&what) noexcept {
	return Attribute{"error", std::forward<Str>(what)};
}

inline Attribute Err(const std::exception &e) noexcept {
	return Err(e.what());
}

inline bool Attribute::operator==(const Attribute &other) const noexcept {
	return key == other.key && value == other.value;
}

} // namespace slog
