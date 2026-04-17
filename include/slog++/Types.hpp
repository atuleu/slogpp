#pragma once

#include "utils/ContainerReference.hpp"

#include <chrono>
#include <memory>
#include <string>
#include <variant>

#ifndef SLOGPP_NO_DETAILS_STRING
#include "details/String.hpp"
#endif

namespace slog {

struct Attribute;

/**
 * Represents a duration in nanosecond count.
 */
typedef std::chrono::nanoseconds DurationT;

/**
 * Represents a time by the number of nanosecond since EPOCH.
 */
typedef std::chrono::
    time_point<std::chrono::system_clock, std::chrono::nanoseconds>
        TimeT;

namespace details {

struct Group {
	utils::ContainerReference<Attribute> attributes;

	virtual ~Group() = default;

protected:
	Group()                         = default;
	Group(const Group &)            = default;
	Group(Group &&)                 = default;
	Group &operator=(const Group &) = default;
	Group &operator=(Group &&)      = default;
};

} // namespace details

typedef std::shared_ptr<details::Group> GroupPtr;
using Buffer = std::string;

#ifndef SLOGPP_NO_DETAILS_STRING
using StringType = details::String;
#define SLOGPP_appendToBuffer(b, str)                                          \
	do {                                                                       \
		b += (str).string_view();                                              \
	} while (0)
#else
using StringType = std::string;
#define SLOGPP_appendToBuffer(b, str)                                          \
	do {                                                                       \
		b += (str);                                                            \
	} while (0)
#endif

using Value = std::variant<
    std::monostate,
    bool,
    int64_t,
    double,
    StringType,
    DurationT,
    TimeT,
    GroupPtr,
    void *>;

class Record;

using Formatter = void (*)(const Record &record, Buffer &);

}; // namespace slog
