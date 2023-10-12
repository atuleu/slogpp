#pragma once

#include "Config.hpp"
#include "Formatters.hpp"
#include "Level.hpp"
#include "Sink.hpp"
#include "utils/ObjectPool.hpp"

#include <array>
#include <bits/types/FILE.h>
#include <cstdio>
#include <future>
#include <memory>
#include <mutex>
#include <system_error>
#include <variant>

namespace slog {

namespace details {

inline static utils::ObjectPool<Buffer> bufferPool{};

template <typename T> class Sink : public virtual slog::Sink {
public:
	using LevelArray = std::array<bool, NumLevels>;

	Sink(const LevelArray &array, Formatter formatter);

	inline void Log(slog::Sink::RecordVariant &&record) override {
		static_cast<T *>(this)->LogImpl(std::move(record));
	}

	inline bool Enabled(Level lvl) const noexcept override {
		auto index = size_t(lvl) + 1;
		if (index >= NumLevels) {
			index = 0;
		}
		return d_levels[index];
	}

	void LogImpl(slog::Sink::RecordVariant &&record) {
		auto buffer = bufferPool.Get();
		buffer->resize(0);
		std::visit(
		    [this, &buffer](auto &&arg) { d_formatter(*arg, *buffer); },
		    record
		);
		static_cast<T *>(this)->Log(std::move(buffer));
	}

	Sink(const Sink &) = default;

	Sink(Sink &&)                 = default;
	Sink &operator=(const Sink &) = default;
	Sink &operator=(Sink &&)      = default;

private:
	std::array<bool, NumLevels> d_levels;
	Formatter                   d_formatter;
};

template <typename T>
class AsyncSink : public virtual slog::Sink,
                  public std::enable_shared_from_this<AsyncSink<T>> {
public:
	void LogAsync(slog::Sink::RecordVariant &&record) {
		auto self = this->shared_from_this();
		std::async([self, record = std::move(record)]() mutable {
			static_cast<T *>(self.get())->LogImpl(std::move(record));
		});
	}
};

template <typename T> class LockedSync {
public:
	LockedSync(T nested) noexcept
	    : d_nested(nested) {}

protected:
	void Log(slog::Sink::RecordVariant &&record) {
		std::scoped_lock<std::mutex> lock(d_mutex);
		d_nested.LogImpl(std::move(record));
	}

private:
	std::mutex d_mutex;
	T          d_nested;
};

} // namespace details

class FileSink : public details::Sink<FileSink> {
public:
	inline FileSink(const FileSinkConfig &config)
	    : Sink(config.levels, config.Formatter()) {
		auto file = std::fopen(config.filepath.c_str(), "a");
		if (file == nullptr) {
			throw std::system_error(errno, std::generic_category());
		}
		d_file = FilePtr(file, [](FILE *f) { std::fclose(f); });
	}

	inline FileSink(const ProgramOutputSinkConfig &config)
	    : Sink(config.levels, config.Formatter()) {
		d_file = FilePtr(config.stdout ? stdout : stderr, [](FILE *f) {});
	}

	void Log(utils::ObjectPool<Buffer>::Ptr &&buffer) {
		std::fwrite(buffer->data(), sizeof(char), buffer->size(), d_file.get());
	}

private:
	using FileCloser = std::function<void(std::FILE *)>;
	using FilePtr    = std::unique_ptr<std::FILE, FileCloser>;

	FilePtr d_file;
};

class AsyncFileSink : public FileSink,
                      public details::AsyncSink<AsyncFileSink> {
public:
	inline AsyncFileSink(const FileSinkConfig &config)
	    : FileSink(config) {}

	inline AsyncFileSink(const ProgramOutputSinkConfig &config)
	    : FileSink(config) {}

	virtual void Log(Sink::RecordVariant &&record) override {
		LogAsync(std::move(record));
	}
};

} // namespace slog
