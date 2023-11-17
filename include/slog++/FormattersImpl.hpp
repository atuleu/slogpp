#pragma once

#include "Attribute.hpp"
#include "Formatters.hpp"
#include "Level.hpp"
#include "Record.hpp"
#include <cctype>
#include <charconv>
#include <ctime>
#include <limits>
#include <type_traits>

namespace slog {

namespace details {

inline void FormatTo(bool value, Buffer &buffer) {
	buffer += value ? "true" : "false";
}

inline void FormatTo(int64_t value, Buffer &buffer) {
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

inline void FormatTo(double value, Buffer &buffer) {
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

inline void TextFormatTo(const std::string &value, Buffer &buffer) {
	bool quote = std::find_if(value.begin(), value.end(), [](char c) {
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

inline bool needEscaping(const std::string &value) noexcept {
	return std::any_of(value.begin(), value.end(), [](unsigned char c) {
		return c == '\\' || c == '\"' || c < 0x20 || c > 0x7f;
	});
}

inline void appendHex(uint16_t ch, Buffer &buffer) {
	const char hexDigits[] =
	    "000102030405060708090a0b0c0d0e0f"
	    "101112131415161718191a1b1c1d1e1f"
	    "202122232425262728292a2b2c2d2e2f"
	    "303132333435363738393a3b3c3d3e3f"
	    "404142434445464748494a4b4c4d4e4f"
	    "505152535455565758595a5b5c5d5e5f"
	    "606162636465666768696a6b6c6d6e6f"
	    "707172737475767778797a7b7c7d7e7f"
	    "808182838485868788898a8b8c8d8e8f"
	    "909192939495969798999a9b9c9d9e9f"
	    "a0a1a2a3a4a5a6a7a8a9aaabacadaeaf"
	    "b0b1b2b3b4b5b6b7b8b9babbbcbdbebf"
	    "c0c1c2c3c4c5c6c7c8c9cacbcccdcecf"
	    "d0d1d2d3d4d5d6d7d8d9dadbdcdddedf"
	    "e0e1e2e3e4e5e6e7e8e9eaebecedeeef"
	    "f0f1f2f3f4f5f6f7f8f9fafbfcfdfeff";

	buffer += "\\u";
	size_t lower = ch & 0xff;
	size_t upper = ch >> 8;

	buffer.push_back(hexDigits[2 * upper]);
	buffer.push_back(hexDigits[2 * upper + 1]);
	buffer.push_back(hexDigits[2 * lower]);
	buffer.push_back(hexDigits[2 * lower + 1]);
}

inline void JSONFormatTo(const std::string &value, Buffer &buffer) {
	buffer.reserve(buffer.size() + value.size() + 2);
	buffer.push_back('\"');

	if (!needEscaping(value)) {
		buffer += value;
		buffer.push_back('\"');
		return;
	}

	for (char ch : value) {
		switch (ch) {
		case '\"':
			buffer += "\\\"";
			break;
		case '\\':
			buffer += "\\\\";
			break;
		case '\b':
			buffer += "\\b";
			break;
		case '\f':
			buffer += "\\f";
			break;
		case '\n':
			buffer += "\\n";
			break;
		case '\r':
			buffer += "\\r";
			break;
		case '\t':
			buffer += "\\t";
			break;

		default:
			if (ch < 0x20) {
				appendHex(ch, buffer);
			} else if (ch < 0x80) {
				buffer.push_back(ch);
			} else {
				appendHex(ch, buffer);
			}
		}
	}

	buffer.push_back('\"');
}

    inline void FormatTo(const DurationT &duration, Buffer &buffer) {
		constexpr static int64_t us = 1000;
		constexpr static int64_t ms = 1000 * us;
		constexpr static int64_t s  = 1000 * ms;
		constexpr static int64_t m  = 60 * s;
		constexpr static int64_t h  = 60 * m;

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
				FormatTo(ns, buffer);
				buffer += "ns";
				break;
			}

			if (ns < ms) {
				FormatTo(ns / 1.0e3, buffer);
				buffer += "µs";
				break;
			}

			if (ns < s) {
				FormatTo(ns / 1.0e6, buffer);
				buffer += "ms";
				break;
			}

			int64_t minutes = ns / m;

			if (minutes == 0) {
				FormatTo(ns / 1.0e9, buffer);
				buffer += "s";
				break;
			}
			double  seconds = (ns % m) / 1.0e9;
			int64_t hours   = minutes / 60;
			if (hours == 0) {
				FormatTo(minutes, buffer);
				buffer += "m";
				FormatTo(seconds, buffer);
				buffer += "s";
				break;
			}

			minutes = minutes % 60;
			FormatTo(hours, buffer);
			buffer += "h";
			FormatTo(minutes, buffer);
			buffer += "m";
			FormatTo(seconds, buffer);
			buffer += "s";
		} while (0);
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

	inline void FormatTo(const TimeT &time, Buffer &buffer) {
		using namespace std::chrono;

		const auto time_s = time_point_cast<seconds>(time);
		int64_t    nanos  = (time - time_s).count();
		const auto c_time = system_clock::to_time_t(time_s);

		size_t start = buffer.size();

		constexpr static size_t formatCount = 19;
		buffer.resize(
		    buffer.size() + formatCount
		); // we will put exactly 19 char

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
	}

	inline const std::string &levelName(Level level) {
		static std::string names[NumLevels] = {
		    "UNKNOWN",                                  //
		    "TRACE",   "TRACE_1", "TRACE_2", "TRACE_3", //
		    "DEBUG",   "DEBUG_1", "DEBUG_2", "DEBUG_3", //
		    "INFO",    "INFO_1",  "INFO_2",  "INFO_3",  //
		    "WARN",    "WARN_1",  "WARN_2",  "WARN_3",  //
		    "ERROR",   "ERROR_1", "ERROR_2", "ERROR_3", //
		    "FATAL",
		};
		size_t idx(size_t(level) + 1);
		if (idx >= NumLevels) {
			return names[0];
		}
		return names[idx];
	}

	inline const std::string &levelColor(Level level) {
		static std::string codes[NumLevels] = {
		    "", // Level::Unknown  default
		    "",
		    "",
		    "",
		    "", // Level::Trace default
		    "\033[34m",
		    "\033[34m",
		    "\033[34m",
		    "\033[34m", // Level::Debug Blue on default
		    "\033[36m",
		    "\033[36m",
		    "\033[36m",
		    "\033[36m", // Level::Info Cyan on default
		    "\033[33m",
		    "\033[33m",
		    "\033[33m",
		    "\033[33m", // Level::Warn Yellow on default
		    "\033[31m",
		    "\033[31m",
		    "\033[31m",
		    "\033[31m",    // Level::Error Red on default
		    "\033[37;41m", // Level::Fatal white on red
		};
		size_t idx(size_t(level) + 1);
		if (idx >= NumLevels) {
			return codes[0];
		}
		return codes[idx];
	}

	// helper constant for the visitor #3
	template <class> inline constexpr bool always_false_v = false;

	inline void attributeToJSON(const Attribute &attribute, Buffer &buffer) {
		buffer += "\"" + attribute.key + "\":";
		std::visit(
		    [&buffer](auto &&arg) {
			    using T = std::decay_t<decltype(arg)>; // cast away references
			    if constexpr (std::is_same_v<T, GroupPtr>) { // Is it a group
				    auto prefix = '{';
				    for (const auto &attr : arg->attributes) {
					    buffer += prefix;
					    prefix = ',';
					    attributeToJSON(attr, buffer);
				    }
				    buffer += "}";
			    } else if constexpr (std::is_same_v<
			                             T,
			                             std::string>) { // formatter
				    details::JSONFormatTo(
				        std::forward<decltype(arg)>(arg),
				        buffer
				    );
			    } else if constexpr (std::is_same_v<T, bool> || std::is_same_v<T, int64_t> || std::is_same_v<T, double>) {
				    details::FormatTo(std::forward<decltype(arg)>(arg), buffer);
			    } else {
				    buffer += "\"";
				    details::FormatTo(std::forward<decltype(arg)>(arg), buffer);
				    buffer += "\"";
			    }
		    },
		    attribute.value
		);
	}

	inline void attributeToText(
	    const Attribute   &attribute,
	    const std::string &groupPrefix,
	    Buffer            &buffer
	) {
		auto name = groupPrefix + attribute.key;
		std::visit(
		    [&buffer, &name](auto &&arg) {
			    using T = std::decay_t<decltype(arg)>; // cast away references
			    if constexpr (std::is_same_v<T, GroupPtr>) { // Is it a group
				    auto prefix = name + ".";
				    auto sep    = "";
				    for (const auto &attr : arg->attributes) {
					    buffer += sep;
					    sep = " ";
					    attributeToText(attr, prefix, buffer);
				    }
			    } else if constexpr (std::is_same_v<T, std::string>) {
				    buffer += name += "=";
				    details::TextFormatTo(
				        std::forward<decltype(arg)>(arg),
				        buffer
				    );
			    } else { // formatter
				    buffer += name + "=";
				    details::FormatTo(std::forward<decltype(arg)>(arg), buffer);
			    }
		    },
		    attribute.value
		);
	}

	inline void attributeToTree(
	    const Attribute   &attribute,
	    const std::string &parentPrefix,
	    bool               isLast,
	    Buffer            &buffer
	) {
		std::string prefix = parentPrefix + (isLast ? "└── " : "├── ");

		std::visit(
		    [&, prefix, parentIsLast = isLast](auto &&arg) {
			    using T = std::decay_t<decltype(arg)>; // cast away references
			    if constexpr (std::is_same_v<T, GroupPtr>) { // Is it a
				                                             // group
				    buffer += prefix + attribute.key;

				    auto newTreePrefix = parentIsLast ? (parentPrefix + "    ")
				                                      : (parentPrefix + "│   ");

				    size_t index = 0;
				    for (const auto &attr : arg->attributes) {
					    bool isLast = ++index == arg->attributes.size();
					    attributeToTree(attr, newTreePrefix, isLast, buffer);
				    }
			    } else if constexpr (std::is_same_v<T, std::string>) {
				    buffer += prefix + attribute.key + "=";
				    details::TextFormatTo(
				        std::forward<decltype(arg)>(arg),
				        buffer
				    );

			    } else {
				    buffer += prefix + attribute.key + "=";
				    details::FormatTo(std::forward<decltype(arg)>(arg), buffer);
			    }
		    },
		    attribute.value
		);
	}

} // namespace details

inline void RecordToJSON(const Record &record, std::string &buffer) {
	buffer += "{\"time\":\"";
	details::FormatTo(record.timestamp, buffer);

	buffer += "\",\"level\":\"" + details::levelName(record.level) + "\"";

	buffer += ",\"message\":";
	details::JSONFormatTo(record.message, buffer);

	for (const Attribute &attribute : record.attributes) {
		buffer += ",";
		details::attributeToJSON(attribute, buffer);
	}

	buffer += "}";
}

inline void RecordToRawText(const Record &record, Buffer &buffer) {
	details::FormatTo(record.timestamp, buffer);

	buffer += " " + details::levelName(record.level) + " ";

	details::TextFormatTo(record.message, buffer);

	for (const Attribute &attribute : record.attributes) {
		buffer += " ";
		details::attributeToText(attribute, "", buffer);
	}
}

inline void RecordToANSIText(const Record &record, Buffer &buffer) {
	details::FormatTo(record.timestamp, buffer);

	buffer += " " + details::levelColor(record.level) +
	          details::levelName(record.level) + "\033[m ";
	details::TextFormatTo(record.message, buffer);

	size_t idx = 0;
	for (const Attribute &attribute : record.attributes) {
		bool isLast = ++idx == record.attributes.size();
		details::attributeToTree(attribute, "\n", isLast, buffer);
	}
}

}; // namespace slog
