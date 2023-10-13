#pragma once

#include "Record.hpp"

#include <algorithm>
#include <chrono>
#include <iterator>

namespace slog {

template <typename Str>
inline Record::Record(Level level, Str &&message) noexcept
    : timestamp{std::chrono::system_clock::now()}
    , level{level}
    , message{std::forward<Str>(message)} {}

template <typename Timestamp, typename Str>
inline Record::Record(Timestamp &&time, Level level, Str &&message) noexcept
    : timestamp{std::forward<Timestamp>(time)}
    , level{level}
    , message{std::forward<Str>(message)} {}

namespace details {

template <
    typename T,
    size_t N,
    typename... U,
    std::enable_if_t<(N + sizeof...(U)) >= 1> * = nullptr>
std::array<T, N + sizeof...(U)>
append(const std::array<T, N> &a, U &&...values) {
	std::array<T, N + sizeof...(U)> result{};
	std::copy(a.begin(), a.end(), result.begin());
	std::size_t index = N - 1;
	((result[++index] = std::forward<U>(values)), ...);
	return result;
}

template <size_t N>
template <typename Str, size_t M, typename... Attributes>
inline Record<N>::Record(
    Level                           level,
    Str                           &&message,
    const std::array<Attribute, M> &copiedAttributes,
    Attributes &&...attributes
) noexcept
    : slog::Record{level, std::forward<Str>(message)}
    , d_data{
          details::append(
              copiedAttributes, std::forward<Attributes>(attributes)...
          ),
      } {
	this->attributes = utils::ContainerReference<Attribute>{d_data};
}

template <size_t N>
template <typename Str, typename... Attributes>
inline Record<N>::Record(
    Level level, Str &&message, Attributes &&...attributes
) noexcept
    : slog::Record{level, std::forward<Str>(message)}
    , d_data{std::forward<Attributes>(attributes)...} {
	this->attributes = utils::ContainerReference<Attribute>(d_data);
}

template <size_t N>
template <typename Timestamp, typename Str, typename... Attributes>
inline Record<N>::Record(
    Timestamp &&timestamp,
    Level       level,
    Str       &&message,
    Attributes &&...attributes
) noexcept
    : slog::
          Record{std::forward<Timestamp>(timestamp), level, std::forward<Str>(message)}
    , d_data{std::forward<Attribute>(attributes)...} {
	this->attributes = utils::ContainerReference<Attribute>(d_data);
}

template <typename Str>
Record<0>::Record(Level level, Str &&message) noexcept
    : slog::Record{level, std::forward<Str>(message)} {}

template <typename Timestamp, typename Str>
Record<0>::Record(Timestamp &&timestamp, Level level, Str &&message) noexcept
    : slog::Record{
          std::forward<Timestamp>(timestamp),
          level,
          std::forward<Str>(message),
      } {}
} // namespace details

} // namespace slog
