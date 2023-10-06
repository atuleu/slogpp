#pragma once

#include "Logger.hpp"

#include <algorithm>

#include "Record.hpp"

#include "Sink.hpp"

namespace slog {

template <size_t N>
template <std::enable_if_t<N == 0, int>>
Logger<N>::Logger(const std::shared_ptr<Sink> &sink)
    : d_sink(sink){};

template <size_t N>
template <std::enable_if_t<N != 0, int>>
Logger<N>::Logger(
    const std::shared_ptr<Sink> &sink, std::array<Attribute, N> &&attributes
)
    : d_sink(sink)
    , d_attributes(attributes) {}

template <size_t N>
template <typename... NewFields>
Logger<N + sizeof...(NewFields)> Logger<N>::With(NewFields &&...fields) const {
	std::array<Attribute, N + sizeof...(fields)> attributes;
	std::copy(d_attributes.begin(), d_attributes.end(), attributes.begin());
	std::size_t index = N - 1;
	((attributes[++index] = fields), ...);

	return Logger<N + sizeof...(fields)>(d_sink, attributes);
}

template <size_t N> Logger<N + 1> Logger<N>::WithError(const char *what) const {
	std::array<Attribute, N + 1> attributes;
	std::copy(d_attributes.begin(), d_attributes.end(), attributes.begin());
	attributes[N] = Attribute{"error", std::string(what)};
	return Logger<N + 1>(d_sink, attributes);
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
	Record record(level, msg, d_attributes.size() + sizeof...(fields));

	record.PushAttributes(d_attributes.begin(), d_attributes.end());

	((record.PushAttribute(std::forward<Fields>(fields))), ...);

	d_sink->Log(std::move(record));
}
} // namespace slog
