#pragma once

#include "Formatters.hpp"
#include "Record.hpp"

namespace slog {

const std::string &levelName(Level level) {
	static std::array<std::string, 22> names = {
	    "UNKNOWN", "TRACE",   "TRACE_1", "TRACE_2", "TRACE_3", "DEBUG",
	    "DEBUG_1", "DEBUG_2", "DEBUG_3", "INFO",    "INFO_1",  "INFO_2",
	    "INFO_3",  "WARN",    "WARN_1",  "WARN_2",  "WARN_3",  "ERROR",
	    "ERROR_1", "ERROR_2", "ERROR_3", "CRITICAL"};
	size_t idx(size_t(level) + 9);
	if (idx >= 22) {
		return names[0];
	}
	return names[idx];
}

// helper constant for the visitor #3
template <class> inline constexpr bool always_false_v = false;

inline void attributeToJSON(const Attribute &attribute, std::string &buffer) {
	buffer += "\"" + attribute.key + "\":";
	std::visit(
	    [&buffer](auto &&arg) {
		    using T = std::decay_t<decltype(arg)>;
		    if constexpr (std::is_same_v<T, int64_t>) {
			    buffer += std::to_string(arg);
		    } else if constexpr (std::is_same_v<T, double>) {
			    buffer += std::to_string(arg);
		    } else if constexpr (std::is_same_v<T, std::string>) {
			    buffer += "\"" + arg + "\"";
		    } else if constexpr (std::is_same_v<T, DurationT>) {
			    buffer += std::to_string(arg.count());
		    } else if constexpr (std::is_same_v<T, TimeT>) {
			    buffer += "\"NOOP\"";
		    } else if constexpr (std::is_same_v<T, GroupPtr>) {
			    auto prefix = '{';
			    for (const auto &attr : *arg) {
				    buffer += prefix;
				    prefix = ',';
				    attributeToJSON(attr, buffer);
			    }
			    buffer += "}";
		    } else if constexpr (std::is_same_v<T, bool>) {
			    buffer += arg ? "true" : "false";
		    } else {
			    static_assert(always_false_v<T>, "non-exhaustive visitor");
		    }
	    },
	    attribute.value
	);
}

inline void RecordToJSON(const RecordBase &record, std::string &buffer) {
	uint64_t nanos   = record.timestamp.time_since_epoch().count();
	uint64_t seconds = nanos / 1000000000UL;
	nanos -= seconds * 1000000000UL;

	buffer +=
	    "{\"time\":" + std::to_string(seconds) + "." + std::to_string(nanos);

	buffer += ",\"level\":\"" + levelName(record.level) + "\"";

	buffer += ",\"message\":\"" + record.message + "\"";

	for (const Attribute &attribute : record) {
		buffer += ",";
		attributeToJSON(attribute, buffer);
	}

	buffer += "}";
}

inline void RecordToText(const RecordBase &record, std::string &buffer) {
	throw std::runtime_error("not yet implemented");
}

}; // namespace slog
