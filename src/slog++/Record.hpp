#pragma once

#include "Attribute.hpp"
#include "Level.hpp"

namespace slog {

class RecordBase : public AttributeContainer {
public:
	TimeT       timestamp;
	Level       level;
	std::string message;

	template <typename Str> RecordBase(Level level, Str &&message) noexcept;

	template <typename Timestamp, typename Str>
	RecordBase(Timestamp &&time, Level level, Str &&message) noexcept;

	virtual ~RecordBase() noexcept = default;
};

template <size_t N> class Record : public RecordBase {
public:
	std::array<Attribute, N> attributes;

	template <typename Str, size_t M, typename... Attributes>
	Record(
	    Level                           level,
	    Str	                       &&message,
	    const std::array<Attribute, M> &copiedAttributes,
	    Attributes &&...attributes
	) noexcept;

	template <typename Str, typename... Attributes>
	Record(Level level, Str &&message, Attributes &&...attributes) noexcept;

	template <typename Timestamp, typename Str, typename... Attributes>
	Record(
	    Timestamp &&timestamp,
	    Level       level,
	    Str       &&message,
	    Attributes &&...attributes
	) noexcept;

	const_iterator  begin() const noexcept override;
	const_iterator  end() const noexcept override;
	size_type       size() const noexcept override;
	const_reference operator[](size_type n) const noexcept override;
};

} // namespace slog

#include "RecordImpl.hpp"
