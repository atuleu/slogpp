#pragma once

#include <algorithm>
#include <condition_variable>
#include <functional>
#include <memory>
#include <mutex>
#include <type_traits>

namespace slog {

namespace utils {

template <
    typename T,
    std::enable_if_t<std::is_nothrow_constructible_v<T>> * = nullptr>
class ObjectPool {
public:
	using ReAnnounce = std::function<void(T *)>;
	using Ptr        = std::unique_ptr<T, ReAnnounce>;

	inline ~ObjectPool() {
		std::unique_lock<std::mutex> lock(d_mutex);
		d_condition.wait(lock, [this]() {
			return d_available.size() == d_objects.size();
		});

		for (auto obj : d_available) {
			delete obj;
		}
	}

	ObjectPool()                              = default;
	// ObjectPool is non-movable non-copyable
	ObjectPool(const ObjectPool &)            = delete;
	ObjectPool(ObjectPool &&)                 = delete;
	ObjectPool &operator=(const ObjectPool &) = delete;
	ObjectPool &operator=(ObjectPool &&)      = delete;

	void Reserve(size_t capacity) {
		std::scoped_lock<std::mutex> lock(d_mutex);

		while (d_objects.size() < capacity) {
			d_available.push_back(d_objects.emplace_back(new T()));
		}
	}

	Ptr Get() noexcept {
		std::scoped_lock<std::mutex> lock(d_mutex);

		if (d_available.empty() == true) {
			d_available.push_back(d_objects.emplace_back(new T()));
		}

		auto ptr = d_available.back();
		d_available.pop_back();

		return {ptr, [this](T *obj) { this->announce(obj); }};
	}

	size_t Available() noexcept {
		std::scoped_lock<std::mutex> lock(d_mutex);
		return d_available.size();
	}

	size_t Capacity() noexcept {
		std::scoped_lock<std::mutex> lock(d_mutex);
		return d_objects.size();
	}

private:
	void announce(T *object) {
		std::scoped_lock<std::mutex> lock(d_mutex);

		d_available.push_back(object);
		d_condition.notify_one();
	}

	std::mutex d_mutex;
	std::condition_variable d_condition;

	std::vector<T *> d_objects;
	std::vector<T *> d_available;
};

} // namespace utils
} // namespace slog
