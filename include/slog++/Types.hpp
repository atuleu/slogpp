#pragma once

#include "details/String.hpp"
#include "utils/ContainerReference.hpp"

#include <chrono>
#include <memory>
#include <string>
#include <variant>

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

using Value = std::variant<
    std::monostate,
    bool,
    int64_t,
    double,
    details::String,
    DurationT,
    TimeT,
    GroupPtr,
    void *>;

using Buffer = std::string;

class Record;

using Formatter = void (*)(const Record &record, Buffer &);

}; // namespace slog
