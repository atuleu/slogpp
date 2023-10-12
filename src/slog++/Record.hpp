#pragma once

#include "Attribute.hpp"
#include "Level.hpp"
#include "Types.hpp"

namespace slog {

class Record {
public:
	TimeT                         timestamp;
	Level                         level;
	std::string                   message;
	ContainerReference<Attribute> attributes;
	virtual ~Record() = default;

	// non-copyable non-movable;
	Record(const Record &)            = delete;
	Record(Record &&)                 = delete;
	Record &operator=(const Record &) = delete;
	Record &operator=(Record &&)      = delete;

protected:
	template <typename Str> Record(Level level, Str &&message) noexcept;

	template <typename Timestamp, typename Str>
	Record(Timestamp &&time, Level level, Str &&message) noexcept;
};

namespace details {
template <size_t N> class Record : public slog::Record {
public:
	/**
	 * Constructor from a mix of copied and perfect forwarded Attributes, to in
	 * Logger<N> with N > 0.
	 */
	template <typename Str, size_t M, typename... Attributes>
	Record(
	    Level                           level,
	    Str	                       &&message,
	    const std::array<Attribute, M> &copiedAttributes,
	    Attributes &&...attributes
	) noexcept;

	/**
	 * Constructor for only forwarded attributes. Used by Logger<0>.
	 */
	template <typename Str, typename... Attributes>
	Record(Level level, Str &&message, Attributes &&...attributes) noexcept;

	/**
	 * Constructor with custom timestamp. Only used for unit testing purpose.
	 */
	template <typename Timestamp, typename Str, typename... Attributes>
	Record(
	    Timestamp &&timestamp,
	    Level       level,
	    Str       &&message,
	    Attributes &&...attributes
	) noexcept;

private:
	std::array<Attribute, N> d_data;
};

template <> class Record<0> : public slog::Record {
public:
	/**
	 * Constructor for only forwarded attributes. Used by Logger<0>.
	 */
	template <typename Str> Record(Level level, Str &&message) noexcept;

	/**
	 * Constructor with custom timestamp. Only used for unit testing purpose.
	 */
	template <typename Timestamp, typename Str>
	Record(Timestamp &&timestamp, Level level, Str &&message) noexcept;
};

} // namespace details

} // namespace slog

#include "RecordImpl.hpp"
