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

inline void attributeToJSON(const Attribute &attribute, std::string &buffer) {}

inline void RecordToJSON(const Record &record, std::string &buffer) {
	uint64_t nanos   = record.timestamp.time_since_epoch().count();
	uint64_t seconds = nanos / 1000000000UL;
	nanos -= seconds * 1000000000UL;

	buffer +=
	    "{\"time\":" + std::to_string(seconds) + "." + std::to_string(nanos);

	buffer += ",\"level\":\"" + levelName(record.level) + "\"";

	buffer += ",\"message\":\"" + record.message + "\"";

	for (const Attribute &attribute : record.attributes) {
		attributeToJSON(attribute, buffer);
	}

	buffer += "}";
}

inline void RecordToText(const Record &record, std::string &buffer) {
	throw std::runtime_error("not yet implemented");
}

}; // namespace slog
