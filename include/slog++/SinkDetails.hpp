#pragma once

#include "Formatters.hpp"
#include "Level.hpp"
#include "Sink.hpp"
#include "utils/ObjectPool.hpp"
#include "utils/ThreadPool.hpp"

#include <array>
#include <future>
#include <memory>
#include <mutex>

namespace slog {

namespace details {

inline static utils::ObjectPool<Buffer> bufferPool{};
inline static utils::ThreadPool         threadPool{};

enum ConcurencyMode {
	Unsafe      = 0,
	MTSafe      = 1,
	Async       = 2,
	AsyncMtSafe = 3,
};

template <typename T, ConcurencyMode CM> class Sink : public slog::Sink {
public:
	using LevelArray = std::array<bool, NumLevels>;

	inline Sink(const LevelArray &levels, Formatter formatter)
	    : d_levels{levels}
	    , d_formatter{formatter} {}

	inline bool AllocateOnStack() const noexcept override {
		return (CM & Async) != 0;
	}

	inline bool Enabled(Level lvl) const noexcept override {
		auto index = size_t(lvl) + 1;
		if (index >= NumLevels) {
			index = 0;
		}
		return d_levels[index];
	}

	inline void From(Level lvl) noexcept override {
		auto index = size_t(lvl) + 1;
		for (size_t i = 1; i < NumLevels; i++) {
			d_levels[i] = i >= index;
		}
	}

	inline void Set(Level lvl, bool enabled) noexcept override {
		auto index = size_t(lvl) + 1;
		if (index >= NumLevels) {
			return;
		}
		d_levels[index] = enabled;
	}

	inline void Log(slog::Sink::RecordVariant &&record) override {
		static_cast<T *>(this)->LogImpl(std::move(record));
	}

	void LogImpl(slog::Sink::RecordVariant &&record) {
		Buffer buffer;
		buffer.reserve(256);
		std::visit(
		    [this, &buffer](auto &&arg) { d_formatter(*arg, buffer); },
		    record
		);
		static_cast<T *>(this)->Log(buffer);
	}

	Sink(const Sink &) = default;

	Sink(Sink &&)                 = default;
	Sink &operator=(const Sink &) = default;
	Sink &operator=(Sink &&)      = default;

private:
	LevelArray d_levels;
	Formatter  d_formatter;
};

template <typename T> class Sink<T, MTSafe> : public Sink<T, Unsafe> {
public:
	template <typename... Args>
	inline Sink(Args &&...args)
	    : Sink<T, Unsafe>(std::forward<Args>(args)...) {}

	void Log(slog::Sink::RecordVariant &&record) override {
		std::scoped_lock<std::mutex> lock(d_mutex);
		static_cast<T *>(this)->LogImpl(std::move(record));
	}

protected:
	std::mutex d_mutex;
};

template <typename T>
class Sink<T, Async> : public Sink<T, Unsafe>,
                       public std::enable_shared_from_this<Sink<T, Async>> {
public:
	template <typename... Args>
	inline Sink(Args &&...args)
	    : Sink<T, Unsafe>(std::forward<Args>(args)...) {}

	void Log(slog::Sink::RecordVariant &&record) override {
		auto self = this->shared_from_this();
		threadPool.Queue(
		    [self](slog::Sink::RecordVariant &&record) {
			    static_cast<T *>(self.get())->LogImpl(std::move(record));
		    },
		    std::move(record)
		);
	}
};

template <typename T>
class Sink<T, AsyncMtSafe>
    : public Sink<T, Unsafe>,
      public std::enable_shared_from_this<Sink<T, AsyncMtSafe>> {
public:
	template <typename... Args>
	inline Sink(Args &&...args)
	    : Sink<T, Unsafe>(std::forward<Args>(args)...) {}

	void Log(slog::Sink::RecordVariant &&record) override {
		auto self = this->shared_from_this();
		threadPool.Queue(
		    [self](slog::Sink::RecordVariant &&record) {
			    std::scoped_lock<std::mutex> lock(self->d_mutex);
			    static_cast<T *>(self.get())->LogImpl(std::move(record));
		    },
		    std::move(record)
		);
	}

private:
	std::mutex d_mutex;
};

} // namespace details

} // namespace slog
