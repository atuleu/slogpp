#pragma once

#include "Attribute.hpp"
#include <memory>
#include <type_traits>

#include "Level.hpp"

namespace slog {
class Sink;

template <size_t N> class Logger {
public:
	template <size_t M = N, std::enable_if_t<M == 0, bool> = true>
	Logger(const std::shared_ptr<Sink> &sink);

	template <size_t M = N, std::enable_if_t<M >= 1, bool> = true>
	Logger(
	    const std::shared_ptr<Sink> &sink, std::array<Attribute, N> &&attributes
	);

	template <typename... Fields>
	Logger<N + sizeof...(Fields)> With(Fields &&...fields) const;

	Logger<N + 1> WithError(const char *what) const;

	Logger<N + 1> WithError(const std::exception &e) const;

	template <typename Str, typename... Fields>
	void Log(Level level, const Str &msg, Fields &&...fields) const;

	template <typename Str, typename... Fields>
	inline void Trace(const Str &msg, Fields &&...fields) const {
		Log(Level::Trace, msg, std::forward<Fields>(fields)...);
	};

	template <typename Str, typename... Fields>
	inline void Debug(const Str &msg, Fields &&...fields) const {
		Log(Level::Debug, msg, std::forward<Fields>(fields)...);
	};

	template <typename Str, typename... Fields>
	inline void Info(const Str &msg, Fields &&...fields) const {
		Log(Level::Info, msg, std::forward<Fields>(fields)...);
	};

	template <typename Str, typename... Fields>
	inline void Warn(const Str &msg, Fields &&...fields) const {
		Log(Level::Warn, msg, std::forward<Fields>(fields)...);
	};

	template <typename Str, typename... Fields>
	inline void Error(const Str &msg, Fields &&...fields) const {
		Log(Level::Error, msg, std::forward<Fields>(fields)...);
	};

	template <typename Str, typename... Fields>
	inline void Critical(const Str &msg, Fields &&...fields) const {
		Log(Level::Critical, msg, std::forward<Fields>(fields)...);
	};

private:
	std::shared_ptr<Sink>    d_sink;
	std::array<Attribute, N> d_attributes;
};

} // namespace slog

#include "LoggerImpl.hpp"
