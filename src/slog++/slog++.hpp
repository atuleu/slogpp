#pragma once

#include "Config.hpp"
#include "Logger.hpp"

namespace slog {

template <typename... Options>
std::shared_ptr<Sink> BuildSink(Options &&...options);

inline static Logger<0> defaultLogger = Logger<0>(BuildSink());

inline void SetSink(const std::shared_ptr<Sink> &sink) {
	defaultLogger.SetSink(sink);
}

template <typename... Attributes>
inline Logger<sizeof...(Attributes)> With(Attributes &&...attributes) {
	return defaultLogger.With(std::forward<Attributes>(attributes)...);
}

template <typename Str, typename... Attributes>
inline void Log(Level level, Str &&message, Attributes &&...attributes) {
	defaultLogger.Log(
	    level,
	    std::forward<Str>(message),
	    std::forward<Attributes>(attributes)...
	);
}

template <typename Str, typename... Attributes>
inline void Trace(Str &&message, Attributes &&...attributes) {
	defaultLogger.Trace(
	    std::forward<Str>(message),
	    std::forward<Attributes>(attributes)...
	);
}

template <typename Str, typename... Attributes>
inline void Debug(Str &&message, Attributes &&...attributes) {
	defaultLogger.Debug(
	    std::forward<Str>(message),
	    std::forward<Attributes>(attributes)...
	);
}

template <typename Str, typename... Attributes>
inline void Info(Str &&message, Attributes &&...attributes) {
	defaultLogger.Info(
	    std::forward<Str>(message),
	    std::forward<Attributes>(attributes)...
	);
}

template <typename Str, typename... Attributes>
inline void Warn(Str &&message, Attributes &&...attributes) {
	defaultLogger.Warn(
	    std::forward<Str>(message),
	    std::forward<Attributes>(attributes)...
	);
}

template <typename Str, typename... Attributes>
inline void Error(Str &&message, Attributes &&...attributes) {
	defaultLogger.Error(
	    std::forward<Str>(message),
	    std::forward<Attributes>(attributes)...
	);
}

template <typename Str, typename... Attributes>
inline void Fatal(Str &&message, Attributes &&...attributes) {
	defaultLogger.Fatal(
	    std::forward<Str>(message),
	    std::forward<Attributes>(attributes)...
	);
}

} // namespace slog

#include "slog++Impl.hpp"
