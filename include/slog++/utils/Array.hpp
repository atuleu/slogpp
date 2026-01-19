#pragma once

#include <cstddef>

namespace slog {
namespace details {
template <typename T, size_t N> struct Array {

	T d_data[N];

	T *data() noexcept {
		return d_data;
	}

	const T *data() const noexcept {
		return d_data;
	}

	constexpr size_t size() const noexcept {
		return N;
	}

	T *begin() noexcept {
		return d_data;
	}

	const T *begin() const noexcept {
		return d_data;
	}

	const T *cbegin() const noexcept {
		return d_data;
	}

	T *end() noexcept {
		return d_data + N;
	}

	const T *end() const noexcept {
		return d_data + N;
	}

	const T *cend() const noexcept {
		return d_data + N;
	}

	T &operator[](size_t pos) {
		return d_data[pos];
	}

	const T &operator[](size_t pos) const {
		return d_data[pos];
	}
};

template <typename T> struct Array<T, 0> {
	const T *begin() const noexcept {
		return nullptr;
	}

	const T *end() const noexcept {
		return nullptr;
	}

	constexpr size_t size() const noexcept {
		return 0;
	}
};

} // namespace details
} // namespace slog
