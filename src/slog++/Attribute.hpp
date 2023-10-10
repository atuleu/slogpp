#pragma once

#include <chrono>
#include <cstdint>
#include <memory>
#include <string>
#include <type_traits>
#include <variant>
#include <vector>

namespace slog {

struct Attribute;

/**
 * Represents a duration in nanosecond count.
 */
typedef std::chrono::nanoseconds DurationT;

/**
 * Represents a time by the number of nanosecond since EPOCH.
 */
typedef std::chrono::
    time_point<std::chrono::system_clock, std::chrono::nanoseconds>
        TimeT;

namespace details {

template <class T>
auto test_duration_castable(int) -> decltype(
    std::chrono::duration_cast<DurationT>(std::declval<T>()), std::true_type{}
);

template <class> auto test_duration_castable(...) -> std::false_type;

template <class From>
struct is_duration_castable
    : std::integral_constant<
          bool,
          (decltype(test_duration_castable<From>(0))::value)> {};

template <class T>
auto test_time_castable(int) -> decltype(
    std::chrono::time_point_cast<DurationT>(std::declval<T>()), std::true_type{}
);

template <class> auto test_time_castable(...) -> std::false_type;

template <class From>
struct is_time_castable : std::integral_constant<
                              bool,
                              (decltype(test_time_castable<From>(0))::value)> {
};

} // namespace details

typedef std::shared_ptr<std::vector<Attribute>> GroupPtr;

using Value = std::
    variant<bool, int64_t, double, std::string, DurationT, TimeT, GroupPtr>;

struct Attribute {
	std::string key;
	Value       value;

	bool operator==(const Attribute &other) const noexcept;
};

template <typename Str>
Attribute Bool(Str &&key, bool value);

template <
    typename Str,
    typename Integer,
    std::enable_if_t<std::is_integral_v<std::decay_t<Integer>>> * = nullptr>
Attribute Int(Str &&key, Integer value) noexcept;

template <
    typename Str,
    typename Floating,
    std::enable_if_t<std::is_floating_point_v<Floating>> * = nullptr>
Attribute Float(Str &&key, Floating value) noexcept;

template <
    typename Str,
    typename T,
    std::enable_if_t<std::is_convertible_v<T, std::string>> * = nullptr>
Attribute String(Str &&key, T &&value) noexcept;

template <
    typename Str,
    typename DurationType,
    std::enable_if_t<details::is_duration_castable<DurationType>::value> * =
        nullptr>
Attribute Duration(Str &&key, DurationType &&value) noexcept;

template <
    typename Str,
    typename Timepoint,
    std::enable_if_t<details::is_time_castable<Timepoint>::value> * = nullptr>
Attribute Time(Str &&key, Timepoint &&timepoint) noexcept;

template <typename Str, typename... Attributes>
Attribute Group(Str &&key, Attributes &&...attributes) noexcept;

template <
    typename Str,
    std::enable_if_t<std::is_convertible_v<Str, std::string>, bool> = true>
Attribute Error(Str &&what) noexcept;

Attribute Error(const std::exception &e) noexcept;

} // namespace slog

#include "AttributeImpl.hpp"
