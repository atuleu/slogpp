#pragma once

#include "Record.hpp"
#include <algorithm>
#include <chrono>
#include <iterator>

namespace slog {

template <typename Str>
inline RecordBase::RecordBase(Level level, Str &&message) noexcept
    : timestamp(std::chrono::system_clock::now())
    , level(level)
    , message(std::forward<Str>(message)) {}

template <typename Timestamp, typename Str>
inline RecordBase::RecordBase(
    Timestamp &&time, Level level, Str &&message
) noexcept
    : timestamp(std::forward<Timestamp>(time))
    , level(level)
    , message(std::forward<Str>(message)) {}

namespace details {
template <typename T, size_t N, typename... U>
Array<T, N + sizeof...(U)>
append(const std::array<T, N> &array, U &&...values) {

	Array<T, N + sizeof...(U)> result;

	std::copy(array.begin(), array.end(), result.begin());

	std::size_t index = N - 1;
	((result[++index] = std::forward<U>(values)), ...);

	return result;
}

} // namespace details

template <size_t N>
template <typename Str, size_t M, typename... Attributes>
inline Record<N>::Record(
    Level                           level,
    Str                           &&message,
    const std::array<Attribute, M> &copiedAttributes,
    Attributes &&...attributes
) noexcept
    : RecordBase{level, std::forward<Str>(message)}
    , Array<Attribute, N>{
          details::append(
              copiedAttributes, std::forward<Attributes>(attributes)...
          ),
      } {}

template <size_t N>
template <typename Str, typename... Attributes>
inline Record<N>::Record(
    Level level, Str &&message, Attributes &&...attributes
) noexcept
    : RecordBase(level, std::forward<Str>(message))
    , Array<Attribute, N>(std::forward<Attributes>(attributes)...) {}

template <size_t N>
template <typename Timestamp, typename Str, typename... Attributes>
inline Record<N>::Record(
    Timestamp &&timestamp,
    Level       level,
    Str       &&message,
    Attributes &&...attributes
) noexcept
    : RecordBase{std::forward<Timestamp>(timestamp), level, std::forward<Str>(message)}
    , Array<Attribute, N>(std::forward<Attribute>(attributes)...) {}

template <typename Str>
Record<0>::Record(Level level, Str &&message) noexcept
    : RecordBase{level, std::forward<Str>(message)} {}

template <typename Timestamp, typename Str>
Record<0>::Record(Timestamp &&timestamp, Level level, Str &&message) noexcept
    : RecordBase{
          std::forward<Timestamp>(timestamp),
          level,
          std::forward<Str>(message),
      } {}

    } // namespace slog
