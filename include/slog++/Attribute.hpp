#pragma once

#include "Types.hpp"

#include <type_traits>

namespace slog {

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

struct Attribute {
	std::string key;
	Value       value;

	bool operator==(const Attribute &other) const noexcept;
};

namespace details {

template <size_t N> class GroupData : public Group {
public:
	template <typename... Attributes>
	inline GroupData(Attributes &&...attributes)
	    : d_data{std::forward<Attributes>(attributes)...} {
		this->attributes = utils::ContainerReference<Attribute>(d_data);
	};

private:
	std::array<Attribute, N> d_data;
};

template <> class GroupData<0> : public Group {};

} // namespace details

template <typename Str> Attribute Bool(Str &&key, bool value);

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
Attribute Err(Str &&what) noexcept;

Attribute Err(const std::exception &e) noexcept;

} // namespace slog

#include "AttributeImpl.hpp"
