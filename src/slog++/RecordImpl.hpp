#pragma once

#include "Record.hpp"
#include <algorithm>
#include <chrono>
#include <iterator>

namespace slog {

template <typename Str, size_t N, typename... Attributes>
inline Record::Record(
    Level                           lvl,
    Str                           &&msg,
    const std::array<Attribute, N> &copiedAttributes,
    Attributes &&...attributes
)
    : timestamp(std::chrono::system_clock::now())
    , level(lvl)
    , message(std::forward<Str>(msg)) {
	this->attributes.reserve(N + sizeof...(attributes));
	std::copy(
	    copiedAttributes.begin(),
	    copiedAttributes.end(),
	    std::back_inserter(this->attributes)
	);
	this->attributes.push_back(std::forward<Attributes>(attributes)...);
}

template <typename Str, typename... Attributes>
inline Record::Record(Level lvl, Str &&msg, Attributes &&...attributes)
    : timestamp(std::chrono::system_clock::now())
    , level(lvl)
    , message(std::forward<Str>(msg)) {
	this->attributes.reserve(sizeof...(attributes));
	(this->attributes.push_back(std::forward<Attributes>(attributes)), ...);
}

template <typename Timestamp, typename Str, typename... Attributes>
inline Record::Record(
    Timestamp &&time, Level lvl, Str &&msg, Attributes &&...attributes
)
    : timestamp(std::forward<Timestamp>(time))
    , level(lvl)
    , message(std::forward<Str>(msg)) {
	this->attributes.reserve(sizeof...(attributes));
	(this->attributes.push_back(std::forward<Attributes>(attributes)), ...);
}

} // namespace slog
