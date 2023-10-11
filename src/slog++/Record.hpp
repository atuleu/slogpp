#pragma once

#include "Attribute.hpp"
#include "Level.hpp"

namespace slog {

class RecordBase : virtual public Container<Attribute> {
public:
	TimeT       timestamp;
	Level       level;
	std::string message;

	template <typename Str> RecordBase(Level level, Str &&message) noexcept;

	template <typename Timestamp, typename Str>
	RecordBase(Timestamp &&time, Level level, Str &&message) noexcept;
};

template <size_t N>
class Record : public RecordBase, public Array<Attribute, N> {
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

	// forward all AttributeContainer information to AttributeArray.
	using SArray = Array<Attribute, N>;
	using SArray::begin;
	using SArray::cbegin;
	using SArray::cend;
	using SArray::end;
	using SArray::size;
	using SArray::operator[];
};

template <> class Record<0> : public RecordBase {
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

	const_iterator cbegin() const noexcept override {
		return nullptr;
	}

	const_iterator cend() const noexcept override {
		return nullptr;
	}

	const_iterator begin() const noexcept override {
		return nullptr;
	}

	const_iterator end() const noexcept override {
		return nullptr;
	}

	iterator begin() noexcept override {
		return nullptr;
	}

	iterator end() noexcept override {
		return nullptr;
	}

	size_type size() const noexcept override {
		return 0;
	}

	reference operator[](size_type n) noexcept override {
		return *(static_cast<Attribute *>(nullptr));
	}

	const_reference operator[](size_type n) const noexcept override {
		return *(static_cast<Attribute *>(nullptr));
	}
};

} // namespace slog

#include "RecordImpl.hpp"
