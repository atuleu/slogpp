#pragma once

#include "Logger.hpp"
#include "Record.hpp"
#include "Sink.hpp"

#include <algorithm>

namespace slog {

template <size_t N>
inline Logger<N>::Logger(std::shared_ptr<Sink> sink) noexcept
    : d_sink(std::move(sink)){};

template <size_t N>
template <typename... Attributes>
inline Logger<N + sizeof...(Attributes)>
Logger<N>::With(Attributes &&...attributes) const noexcept {
	Logger<N + sizeof...(Attributes)> result{d_sink};
	std::copy(
	    d_attributes.begin(),
	    d_attributes.end(),
	    result.d_attributes.begin()
	);
	std::size_t index = N - 1;
	((result.d_attributes[++index] = std::forward<Attributes>(attributes)),
	 ...);

	return result;
}

template <size_t N>
template <typename Str, typename... Attributes>
inline void
Logger<N>::Log(Level level, Str &&msg, Attributes &&...attributes) const {

	// early discard the entry
	if (!d_sink || !d_sink->Enabled(level)) {
		return;
	}

	// build the record.
	Record<N + sizeof...(Attributes)> record(
	    level,
	    std::forward<Str>(msg),
	    d_attributes,
	    std::forward<Attributes>(attributes)...
	);

	d_sink->Log(record);
}

template <>
template <typename Str, typename... Attributes>
inline void
Logger<0>::Log(Level level, Str &&msg, Attributes &&...attributes) const {

	// early discard the entry
	if (!d_sink || !d_sink->Enabled(level)) {
		return;
	}

	// build the record.
	Record<sizeof...(Attributes)> record(
	    level,
	    std::forward<Str>(msg),
	    std::forward<Attributes>(attributes)...
	);

	d_sink->Log(record);
}

} // namespace slog
