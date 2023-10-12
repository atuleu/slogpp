#pragma once

#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <utility>
#include <vector>

namespace slog {
namespace utils {

class ThreadPool {
public:
	~ThreadPool() {
		this->SetSize(0);
	}

	ThreadPool() = default;

	ThreadPool(const ThreadPool &)            = delete;
	ThreadPool(ThreadPool &&)                 = delete;
	ThreadPool &operator=(const ThreadPool &) = delete;
	ThreadPool &operator=(ThreadPool &&)      = delete;

	using Job = std::function<void()>;

	template <typename Function, typename... Args>
	void Queue(Function &&f, Args &&...args) {
		{
			std::scoped_lock<std::mutex> lock(d_mutex);

			if (d_threads.empty()) {
				std::forward<Function>(f)(std::forward<Args>(args)...);
				return;
			}

			auto job =
			    [f, args = std::tuple(std::forward<Args>(args)...)]() mutable {
				    std::apply(
				        [f](auto &&...args) { f(std::forward<Args>(args)...); },
				        std::move(args)
				    );
			    };
		}
		d_conditions.notify_one();
	}

	inline void SetSize(size_t size) {

		{
			std::scoped_lock<std::mutex> lock(d_mutex);
			d_wantedSize = size;
		}

		while (size < d_threads.size()) {
			d_conditions.notify_all();
			d_threads.back().join();
			d_threads.pop_back();
		}

		while (size > d_threads.size()) {
			int privateID = d_threads.size();
			d_threads.emplace_back([this, privateID] {
				this->waitAndProcess(privateID);
			});
		}
	}

private:
	inline bool shouldQuit(int privateID) const noexcept {
		return privateID >= d_wantedSize;
	}

	inline bool hasJob() const noexcept {
		return !d_queue.empty();
	}

	inline void waitAndProcess(int privateID) {
		std::unique_lock<std::mutex> lock(d_mutex);

		d_conditions.wait(lock, [this, privateID] {
			return shouldQuit(privateID) || hasJob();
		});

		if (shouldQuit(privateID)) {
			return;
		}

		auto &job = d_queue.front();
		job();
		d_queue.pop();
	}

	std::mutex              d_mutex;
	std::condition_variable d_conditions;

	std::vector<std::thread> d_threads;

	size_t d_wantedSize = 0;

	std::queue<Job> d_queue;
};

} // namespace utils
} // namespace slog
