#pragma once

#include "Attribute.hpp"
#include "Formatters.hpp"
#include "Record.hpp"

namespace slog{

namespace details {

inline std::string ToString(bool value) {
	return value ? "true" : "false";
}

inline std::string ToString(int64_t value) {
	return std::to_string(value);
}

inline std::string ToString(double value) {
	return std::to_string(value);
}

inline std::string ToString(const std::string &value) {
	// TODO: quote escaping
	return value;
}

inline std::string ToString(const DurationT &value) {
	return "NOOP";
}

inline std::string ToString(const TimeT &value) {
	return "NOOP";
}

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
		    using T = std::decay_t<decltype(arg)>;       // cast away references
		    if constexpr (std::is_same_v<T, GroupPtr>) { // Is it a group
			    auto prefix = '{';
			    for (const auto &attr : *arg) {
				    buffer += prefix;
				    prefix = ',';
				    attributeToJSON(attr, buffer);
			    }
			    buffer += "}";
		    } else { // formatter
			    buffer += details::ToString(std::forward<decltype(arg)>(arg));
		    }
	    },
	    attribute.value
	);
}

} // namespace details

inline void RecordToJSON(const RecordBase &record, std::string &buffer) {
	buffer += "{\"time\":\"" + details::ToString(record.timestamp) + "\"";

	buffer += ",\"level\":\"" + details::levelName(record.level) + "\"";

	buffer += ",\"message\":\"" + record.message + "\"";

	for (const Attribute &attribute : record) {
		buffer += ",";
		details::attributeToJSON(attribute, buffer);
	}

	buffer += "}";
}

inline void RecordToText(const RecordBase &record, std::string &buffer) {
	throw std::runtime_error("not yet implemented");
}

}; // namespace
