#pragma once

#include "Logger.hpp"
#include "Record.hpp"
#include "Sink.hpp"

#include <algorithm>

namespace slog {

template <size_t N>
template <size_t M, std::enable_if_t<M == 0, bool>>
inline Logger<N>::Logger(const std::shared_ptr<Sink> &sink)
    : d_sink(sink){};

template <size_t N>
template <size_t M, std::enable_if_t<M >= 1, bool>>
inline Logger<N>::Logger(
    const std::shared_ptr<Sink> &sink, std::array<Attribute, N> &&attributes
)
    : d_sink(sink)
    , d_attributes(attributes) {}

template <size_t N>
template <typename... NewFields>
inline Logger<N + sizeof...(NewFields)> Logger<N>::With(NewFields &&...fields
) const {
	std::array<Attribute, N + sizeof...(fields)> attributes;
	std::copy(d_attributes.begin(), d_attributes.end(), attributes.begin());
	std::size_t index = N - 1;
	((attributes[++index] = fields), ...);

	return Logger<N + sizeof...(fields)>(d_sink, std::move(attributes));
}

template <size_t N>
inline Logger<N + 1> Logger<N>::WithError(const char *what) const {
	std::array<Attribute, N + 1> attributes;
	std::copy(d_attributes.begin(), d_attributes.end(), attributes.begin());
	attributes[N] = Error(what);
	return Logger<N + 1>(d_sink, attributes);
}

template <size_t N>
inline Logger<N + 1> Logger<N>::WithError(const std::exception &e) const {
	return WithError(e.what());
}

template <size_t N>
template <typename Str, typename... Fields>
inline void
Logger<N>::Log(Level level, const Str &msg, Fields &&...fields) const {

	// early discard the entry
	if (!d_sink || !d_sink->Enabled(level)) {
		return;
	}

	// build the record.
	Record record(level, msg, d_attributes, std::forward<Fields>(fields)...);

	d_sink->Log(record);
}

template <>
template <typename Str, typename... Fields>
inline void
Logger<0>::Log(Level level, const Str &msg, Fields &&...fields) const {

	// early discard the entry
	if (!d_sink || !d_sink->Enabled(level)) {
		return;
	}

	// build the record.
	Record record(level, msg, std::forward<Fields>(fields)...);

	d_sink->Log(record);
}

} // namespace slog
