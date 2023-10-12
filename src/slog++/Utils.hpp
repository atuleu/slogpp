#pragma once

#include <cstddef>
#include <initializer_list>

namespace slog {

namespace utils {

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

	template <typename Container>
	inline ContainerReference(Container &&container)
	    : d_data(container.data())
	    , d_size(container.size()) {}

	inline ContainerReference()
	    : d_data(nullptr)
	    , d_size(0) {}

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

private:
	pointer   d_data;
	size_type d_size;
};

} // namespace utils
} // namespace slog
