#pragma once

#include "Attribute.hpp"
#include <memory>
#include <type_traits>

namespace slog {
class Sink;

enum Level {
	Trace    = -8,
	Debug    = -4,
	Info     = 0,
	Warn     = 4,
	Error    = 8,
	Critical = 12,
};

template <size_t N> class Logger {
public:
	template <std::enable_if_t<N == 0, int> = 0>
	Logger(const std::shared_ptr<Sink> &sink);

	template <std::enable_if_t<N != 0, int> = 0>
	Logger(const std::shared_ptr<Sink> &sink,
	       std::array<Attribute, N>   &&attributes);

	template <typename... Fields>
	Logger<N + sizeof...(Fields)> With(Fields &&...fields) const;

	Logger<N + 1> WithError(const char *what) const;

	template <typename Str, typename... Fields>
	void Log(Level level, const Str &msg, Fields &&...fields) const;

	template <typename Str, typename... Fields>
	inline void Trace(const Str &msg, Fields &&...fields) const {
		Log(Level::Trace, msg, std::forward(fields...));
	};

	template <typename Str, typename... Fields>
	inline void Debug(const Str &msg, Fields &&...fields) const {
		Log(Level::Debug, msg, std::forward(fields...));
	};

	template <typename Str, typename... Fields>
	inline void Info(const Str &msg, Fields &&...fields) const {
		Log(Level::Info, msg, std::forward(fields...));
	};

	template <typename Str, typename... Fields>
	inline void Warn(const Str &msg, Fields &&...fields) const {
		Log(Level::Warn, msg, std::forward(fields...));
	};

	template <typename Str, typename... Fields>
	inline void Error(const Str &msg, Fields &&...fields) const {
		Log(Level::Error, msg, std::forward(fields...));
	};

	template <typename Str, typename... Fields>
	inline void Critical(const Str &msg, Fields &&...fields) const {
		Log(Level::Critical, msg, std::forward(fields...));
	};

private:
	std::shared_ptr<Sink>    d_sink;
	std::array<Attribute, N> d_attributes;
};

} // namespace slog
