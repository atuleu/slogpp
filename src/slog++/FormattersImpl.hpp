#pragma once

#include "Attribute.hpp"
#include "Formatters.hpp"
#include "Record.hpp"
#include <cctype>
#include <charconv>
#include <ctime>
#include <limits>
#include <type_traits>

namespace slog {

namespace details {

void FormatTo(bool value, bool, Buffer &buffer) {
	buffer += value ? "true" : "false";
}

void FormatTo(int64_t value, bool, Buffer &buffer) {
	auto startSize = buffer.size();
	while (true) {
		buffer.resize(buffer.size() + 32); // may reallocate.

		if (auto [ptr, ec] = std::to_chars(
		        buffer.data() + startSize,
		        buffer.data() + buffer.size(),
		        value
		    );
		    ec == std::errc()) {
			buffer.resize(std::distance(buffer.data(), ptr));
			break;
		}
	}
}

void FormatTo(double value, bool, Buffer &buffer) {
	auto startSize = buffer.size();
	while (true) {
		buffer.resize(buffer.size() + 32); // may reallocate.

		if (auto [ptr, ec] = std::to_chars(
		        buffer.data() + startSize,
		        buffer.data() + buffer.size(),
		        value
		    );
		    ec == std::errc()) {
			buffer.resize(std::distance(buffer.data(), ptr));
			break;
		}
	}
}

void FormatTo(const std::string &value, bool quote, Buffer &buffer) {
	quote = quote || std::find_if(value.begin(), value.end(), [](char c) {
		                 return std::isspace(c) != 0;
	                 }) != value.end();

	buffer.reserve(buffer.size() + value.size() + (quote ? 2 : 0));
	if (quote) {
		buffer.push_back('\"');
	}
	auto prev = '\0';
	for (char ch : value) {
		if (ch == '\"' && prev != '\\') {
			buffer.push_back('\\');
		}
		buffer.push_back(ch);
		prev = ch;
	}

	if (quote) {
		buffer.push_back('\"');
	}
}

void FormatTo(const DurationT &duration, bool quote, Buffer &buffer) {
	constexpr static int64_t us = 1000;
	constexpr static int64_t ms = 1000 * us;
	constexpr static int64_t s  = 1000 * ms;
	constexpr static int64_t m  = 60 * s;
	constexpr static int64_t h  = 60 * m;

	if (quote) {
		buffer.push_back('\"');
	}

	do {
		int64_t ns = duration.count();

		if (ns == 0) {
			buffer += "0s";
			break;
		}

		if (ns == std::numeric_limits<int64_t>::min()) {
			buffer += "-2562047h47m16.854775808s";
			break;
		}

		if (ns < 0) {
			buffer.push_back('-');
			ns = -ns;
		}

		if (ns < us) {
			FormatTo(ns, false, buffer);
			buffer += "ns";
			break;
		}

		if (ns < ms) {
			FormatTo(ns / 1.0e3, false, buffer);
			buffer += "µs";
			break;
		}

		if (ns < s) {
			FormatTo(ns / 1.0e6, false, buffer);
			buffer += "ms";
			break;
		}

		int64_t minutes = ns / m;

		if (minutes == 0) {
			FormatTo(ns / 1.0e9, false, buffer);
			buffer += "s";
			break;
		}
		double  seconds = (ns % m) / 1.0e9;
		int64_t hours   = minutes / 60;
		if (hours == 0) {
			FormatTo(minutes, false, buffer);
			buffer += "m";
			FormatTo(seconds, false, buffer);
			buffer += "s";
			break;
		}

		minutes = minutes % 60;
		FormatTo(hours, false, buffer);
		buffer += "h";
		FormatTo(minutes, false, buffer);
		buffer += "m";
		FormatTo(seconds, false, buffer);
		buffer += "s";
	} while (0);

	if (quote) {
		buffer.push_back('\"');
	}
}

template <typename T> inline constexpr T powImpl(T base, size_t exponent) {
	return exponent == 0 ? 1
	                     : ((exponent % 2 == 0)
	                            ? powImpl(base, exponent / 2) *
	                                  powImpl(base, exponent / 2)
	                            : base * powImpl(base, (exponent - 1) / 2) *
	                                  powImpl(base, (exponent - 1) / 2));
}

template <typename T, T Base, size_t Exp>
using Pow = std::integral_constant<T, powImpl<T>(Base, Exp)>;

template <
    typename Integer,
    size_t N,
    std::enable_if_t<std::is_integral_v<Integer> && 1 <= N && N <= 9> * =
        nullptr>
uint8_t base10Digits(Integer value) {
	uint8_t length = 1;
	if constexpr (N >= 8) {
		length += (value >= Pow<Integer, 10, 8>::value);
	}
	if constexpr (N >= 7) {
		length += (value >= Pow<Integer, 10, 7>::value);
	}
	if constexpr (N >= 6) {
		length += (value >= Pow<Integer, 10, 6>::value);
	}
	if constexpr (N >= 5) {
		length += (value >= Pow<Integer, 10, 5>::value);
	}
	if constexpr (N >= 4) {
		length += (value >= Pow<Integer, 10, 4>::value);
	}
	if constexpr (N >= 3) {
		length += (value >= Pow<Integer, 10, 3>::value);
	}
	if constexpr (N >= 2) {
		length += (value >= Pow<Integer, 10, 2>::value);
	}
	if constexpr (N >= 1) {
		length += (value >= Pow<Integer, 10, 1>::value);
	}
	return length;
}

template <
    typename Integer,
    size_t N,
    std::enable_if_t<std::is_integral_v<Integer> && 1 <= N && N <= 9> * =
        nullptr>
void FormatToPrefix(Integer value, char prefix, Buffer &buffer) {

	if (value < 0 || value >= Pow<Integer, 10, N + 1>::value) {
		throw std::invalid_argument(
		    "value (" + std::to_string(value) +
		    ") should be positive and smaller than " +
		    std::to_string(Pow<Integer, 10, N + 1>::value) +
		    " while printing in " + std::to_string(N) + " digits."
		);
	}

	const auto start  = buffer.size();
	uint8_t    length = base10Digits<Integer, N>(value);

	buffer += std::string(N, prefix);

	std::to_chars(
	    buffer.data() + start + (N - length),
	    buffer.data() + start + N,
	    value
	);
}

void FormatTo(const TimeT &time, bool quote, Buffer &buffer) {
	using namespace std::chrono;
	if (quote) {
		buffer.push_back('\"');
	}

	const auto time_s = time_point_cast<seconds>(time);
	int64_t    nanos  = (time - time_s).count();
	const auto c_time = system_clock::to_time_t(time_s);

	size_t start = buffer.size();

	constexpr static size_t formatCount = 19;
	buffer.resize(buffer.size() + formatCount); // we will put exactly 19 char

	std::strftime(
	    buffer.data() + start,
	    formatCount + 1,
	    "%FT%T",
	    gmtime(&c_time)
	);
	buffer += '.';
	start = buffer.size();
	if (nanos % 1000000 == 0) {
		FormatToPrefix<int64_t, 3>(nanos / 1000000, '0', buffer);
	} else if (nanos % 1000 == 0) {
		FormatToPrefix<int64_t, 6>(nanos / 1000, '0', buffer);
	} else {
		FormatToPrefix<int64_t, 9>(nanos, '0', buffer);
	}

	buffer.push_back('Z');

	if (quote) {
		buffer.push_back('\"');
	}
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

inline void attributeToJSON(const Attribute &attribute, Buffer &buffer) {
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
			    details::FormatTo(
			        std::forward<decltype(arg)>(arg),
			        true,
			        buffer
			    );
		    }
	    },
	    attribute.value
	);
}

inline void attributeToText(
    const Attribute &attribute, const std::string &groupPrefix, Buffer &buffer
) {
	auto name = groupPrefix + attribute.key;
	std::visit(
	    [&buffer, &name](auto &&arg) {
		    using T = std::decay_t<decltype(arg)>;       // cast away references
		    if constexpr (std::is_same_v<T, GroupPtr>) { // Is it a group
			    auto prefix = name + ".";
			    auto sep    = "";
			    for (const auto &attr : *arg) {
				    buffer += sep;
				    sep = " ";
				    attributeToText(attr, prefix, buffer);
			    }
		    } else { // formatter
			    buffer += name + "=";
			    details::FormatTo(
			        std::forward<decltype(arg)>(arg),
			        false,
			        buffer
			    );
		    }
	    },
	    attribute.value
	);
}

} // namespace details

inline void RecordToJSON(const RecordBase &record, std::string &buffer) {
	buffer += "{\"time\":";
	details::FormatTo(record.timestamp, true, buffer);

	buffer += ",\"level\":\"" + details::levelName(record.level) + "\"";

	buffer += ",\"message\":";
	details::FormatTo(record.message, true, buffer);

	for (const Attribute &attribute : record) {
		buffer += ",";
		details::attributeToJSON(attribute, buffer);
	}

	buffer += "}";
}

inline void RecordToText(const RecordBase &record, std::string &buffer) {
	details::FormatTo(record.timestamp, false, buffer);

	buffer += " " + details::levelName(record.level) + " ";

	details::FormatTo(record.message, false, buffer);

	for (const Attribute &attribute : record) {
		buffer += " ";
		details::attributeToText(attribute, "", buffer);
	}
}

}; // namespace slog
