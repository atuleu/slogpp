#pragma once

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

template <typename T> class ContainerReference {
public:
	// Partial const std::Array<Attribute,*> interface
	typedef T              value_type;
	typedef T             *pointer;
	typedef const T       *const_pointer;
	typedef T             &reference;
	typedef const T       &const_reference;
	typedef T             *iterator;
	typedef const T       *const_iterator;
	typedef std::size_t    size_type;
	typedef std::ptrdiff_t difference_type;

	inline const_iterator begin() const noexcept {
		return d_data;
	}

	inline const_iterator end() const noexcept {
		return d_data + d_size;
	}

	inline const_iterator cbegin() const noexcept {
		return d_data;
	}

	inline const_iterator cend() const noexcept {
		return d_data + d_size;
	}

	inline iterator begin() noexcept {
		return d_data;
	}

	inline iterator end() noexcept {
		return d_data + d_size;
	}

	inline size_type size() const noexcept {
		return d_size;
	}

	inline const_reference operator[](size_type n) const noexcept {
		return d_data[n];
	}

	inline reference operator[](size_type n) noexcept {
		return d_data[n];
	}

	template <typename Container>
	inline ContainerReference(Container &&container)
	    : d_data(container.data())
	    , d_size(container.size()) {}

	inline ContainerReference()
	    : d_data(nullptr)
	    , d_size(0) {}

private:
	pointer   d_data;
	size_type d_size;
};

struct GroupT {
	ContainerReference<Attribute> attributes;

	virtual ~GroupT() = default;

protected:
	GroupT()                          = default;
	GroupT(const GroupT &)            = default;
	GroupT(GroupT &&)                 = default;
	GroupT &operator=(const GroupT &) = default;
	GroupT &operator=(GroupT &&)      = default;
};

typedef std::shared_ptr<GroupT> GroupPtr;

using Value = std::
    variant<bool, int64_t, double, std::string, DurationT, TimeT, GroupPtr>;

}; // namespace slog
