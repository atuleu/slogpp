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

typedef std::shared_ptr<std::vector<Attribute>> GroupPtr;

using Value = std::
    variant<bool, int64_t, double, std::string, DurationT, TimeT, GroupPtr>;

struct Attribute {
	std::string key;
	Value       value;

	bool operator==(const Attribute &other) const;
};

template <typename Str> Attribute Bool(const Str &key, bool value);

template <
    typename Str,
    typename Integer,
    std::enable_if_t<std::is_integral_v<Integer>, int> = 0>
Attribute Int(const Str &key, Integer value);

template <
    typename Str,
    typename Floating,
    std::enable_if_t<std::is_floating_point_v<Floating>, int> = 0>
Attribute Float(const Str &key, Floating value);

template <
    typename Str,
    typename T,
    std::enable_if_t<std::is_convertible_v<T &&, std::string>, int> = 0>
Attribute String(const Str &key, T &&value);

template <typename Str, class Rep, class Period = std::ratio<1>>
Attribute
Duration(const Str &key, const std::chrono::duration<Rep, Period> &value);

template <
    typename Str,
    class Duration = typename std::chrono::system_clock::duration>
Attribute Time(
    const Str                                                          &key,
    const std::chrono::time_point<std::chrono::system_clock, Duration> &value
);

template <typename Str, typename... Attributes>
Attribute Group(const Str &key, Attributes &&...attributes);

Attribute Error(const std::exception &);

template <
    typename Str,
    std::enable_if_t<std::is_convertible_v<Str &&, std::string>, bool> = true>
Attribute Error(Str &&what);

} // namespace slog

#include "AttributeImpl.hpp"
