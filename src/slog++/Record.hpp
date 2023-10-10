#pragma once

#include "Attribute.hpp"
#include "Level.hpp"

namespace slog {

struct Record {
	TimeT                  timestamp;
	Level                  level;
	std::string            message;
	std::vector<Attribute> attributes;

	template <typename Str, size_t N, typename... Attributes>
	Record(
	    Level                           lvl,
	    Str	                       &&msg,
	    const std::array<Attribute, N> &copiedAttributes,
	    Attributes &&...attributes
	);

	template <typename Str, typename... Attributes>
	Record(Level lvl, Str &&msg, Attributes &&...attributes);

	template <typename Timestamp, typename Str, typename... Attributes>
	Record(Timestamp &&time, Level lvl, Str &&msg, Attributes &&...attributes);
};

} // namespace slog

#include "RecordImpl.hpp"
