#pragma once

#include "Attribute.hpp"
#include "Types.hpp"

#include <exception>
#include <source_location>
#include <type_traits>

namespace slog {

template <typename Str> inline Attribute Bool(Str &&key, bool value) {
	return Attribute{std::forward<Str>(key), value};
}

template <
    typename Str,
    typename Integer,
    std::enable_if_t<std::is_integral_v<std::decay_t<Integer>>> *>
inline constexpr Attribute Int(Str &&key, Integer value) noexcept {
	return Attribute{std::forward<Str>(key), int64_t(value)};
}

template <
    typename Str,
    typename Floating,
    std::enable_if_t<std::is_floating_point_v<Floating>> *>
inline constexpr Attribute Float(Str &&key, Floating value) noexcept {
	return Attribute{std::forward<Str>(key), double(value)};
}

template <
    typename Str,
    typename T,
    std::enable_if_t<std::is_convertible_v<T, StringType>> *>
inline constexpr Attribute String(Str &&key, T &&value) noexcept {
	return Attribute{std::forward<Str>(key), std::forward<T>(value)};
}

template <
    typename Str,
    typename DurationType,
    std::enable_if_t<details::is_duration_castable<DurationType>::value> *>
inline constexpr Attribute Duration(Str &&key, DurationType &&value) noexcept {
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
inline constexpr Attribute Time(Str &&key, Timepoint &&timepoint) noexcept {
	using namespace std::chrono;
	return Attribute{
	    std::forward<Str>(key),
	    time_point_cast<DurationT>(std::forward<Timepoint>(timepoint)),
	};
}

template <typename Str, typename... Attributes>
inline constexpr Attribute
Group(Str &&key, Attributes &&...attributes) noexcept {
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
inline constexpr Attribute Err(Str &&what) noexcept {
	return Attribute{"error", std::forward<Str>(what)};
}

namespace details {
template <typename T>
concept HasMessageMember = requires(const T &e) {
	{ e.message() } -> std::convertible_to<std::string>;
};
template <typename T>
concept HasTraceMember = requires(const T &e) {
	requires std::is_reference_v<decltype(e.stacktrace())>;
	requires std::is_const_v<
	    std::remove_reference_t<decltype(e.stracktrace())>>;

	{ e.trace().to_string(false) } -> std::convertible_to<std::string>;
};

} // namespace details

template <typename E>
    requires std::derived_from<std::decay_t<E>, std::exception>
inline constexpr Attribute Err(const E &e) noexcept {

	if constexpr (details::HasMessageMember<std::decay_t<E>>) {
		if constexpr (details::HasTraceMember<std::decay_t<E>>) {
			return slog::Group(
			    "error",
			    slog::String("message", e.message()),
			    slog::String("stacktrace", e.trace().to_string(false))
			);
		}
		return Err(e.message());
	}
	return Err(e.what());
}

    inline bool Attribute::operator==(const Attribute &other) const noexcept {
		return key == other.key && value == other.value;
	}

	template <
	    typename Str,
	    typename T,
	    std::enable_if_t<std::is_pointer_v<T>> *>
	Attribute constexpr Pointer(Str && key, T value) noexcept {
		return Attribute{std::forward<Str>(key), static_cast<void *>(value)};
	}

} // namespace slog
