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

template <size_t N>
template <typename Str, size_t M, typename... Attributes>
inline Record<N>::Record(
    Level                           level,
    Str                           &&message,
    const std::array<Attribute, M> &copiedAttributes,
    Attributes &&...attributes
) noexcept
    : RecordBase(level, std::forward<Str>(message)) {
	std::copy(
	    copiedAttributes.begin(),
	    copiedAttributes.end(),
	    this->attributes.begin()
	);
	std::size_t index = M - 1;
	((this->attributes[++index] = std::forward<Attributes>(attributes)), ...);
}

template <size_t N>
template <typename Str, typename... Attributes>
inline Record<N>::Record(
    Level level, Str &&message, Attributes &&...attributes
) noexcept
    : RecordBase(level, std::forward<Str>(message)) {
	std::size_t index = -1;
	((this->attributes[++index] = std::forward<Attributes>(attributes)), ...);
}

template <size_t N>
template <typename Timestamp, typename Str, typename... Attributes>
inline Record<N>::Record(
    Timestamp &&timestamp,
    Level       level,
    Str       &&message,
    Attributes &&...attributes
) noexcept
    : RecordBase(
          std::forward<Timestamp>(timestamp), level, std::forward<Str>(message)
      ) {
	std::size_t index = -1;
	((this->attributes[++index] = std::forward<Attributes>(attributes)), ...);
}

template <size_t N>
inline typename Record<N>::const_iterator Record<N>::begin() const noexcept {
	return attributes.cbegin();
}

template <size_t N>
inline typename Record<N>::const_iterator Record<N>::end() const noexcept {
	return attributes.cend();
}

template <size_t N>
inline typename Record<N>::size_type Record<N>::size() const noexcept {
	return N;
}

template <size_t N>
inline typename Record<N>::const_reference Record<N>::operator[](size_type n
) const noexcept {
	return attributes[n];
}

} // namespace slog
