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

template <typename T> class Container {
public:
	virtual ~Container() = default;

	// Partial const std::Array<Attribute,*> interface
	typedef T              value_type;
	typedef const T       *const_pointer;
	typedef T             &reference;
	typedef const T       &const_reference;
	typedef T             *iterator;
	typedef const T       *const_iterator;
	typedef std::size_t    size_type;
	typedef std::ptrdiff_t difference_type;

	virtual const_iterator  begin() const noexcept                 = 0;
	virtual const_iterator  end() const noexcept                   = 0;
	virtual const_iterator  cbegin() const noexcept                = 0;
	virtual const_iterator  cend() const noexcept                  = 0;
	virtual iterator        begin() noexcept                       = 0;
	virtual iterator        end() noexcept                         = 0;
	virtual size_type       size() const noexcept                  = 0;
	virtual reference       operator[](size_type n) noexcept       = 0;
	virtual const_reference operator[](size_type n) const noexcept = 0;
};

typedef std::shared_ptr<Container<Attribute>> GroupPtr;

using Value = std::
    variant<bool, int64_t, double, std::string, DurationT, TimeT, GroupPtr>;

}; // namespace slog
