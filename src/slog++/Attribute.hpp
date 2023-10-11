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

template <typename T, size_t N, std::enable_if_t<N >= 1> * = nullptr>
class Array : virtual public Container<Attribute> {
public:
	template <typename... U>
	Array(U &&...values)
	    : d_data{std::forward<U>(values)...} {};

	const_iterator cbegin() const noexcept override {
		return d_data;
	}

	const_iterator cend() const noexcept override {
		return d_data + N;
	}

	const_iterator begin() const noexcept override {
		return d_data;
	}

	const_iterator end() const noexcept override {
		return d_data + N;
	}

	iterator begin() noexcept override {
		return d_data;
	}

	iterator end() noexcept override {
		return d_data + N;
	}

	size_type size() const noexcept override {
		return N;
	}

	reference operator[](size_type n) noexcept override {
		return d_data[n];
	}

	const_reference operator[](size_type n) const noexcept override {
		return d_data[n];
	}

private:
	T d_data[N];
};

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
Attribute Error(Str &&what) noexcept;

Attribute Error(const std::exception &e) noexcept;

} // namespace slog

#include "AttributeImpl.hpp"
