#pragma once

#include "Config.hpp"
#include "Logger.hpp"

namespace slog {

template <typename... Options>
std::shared_ptr<Sink> BuildSink(Options &&...options);

Logger<0> &DefaultLogger();

template <typename... Attributes>
inline Logger<sizeof...(Attributes)> With(Attributes &&...attributes) {
	return DefaultLogger().With(std::forward<Attributes>(attributes)...);
}

template <typename Str, typename... Attributes>
inline void Log(Level level, Str &&message, Attributes &&...attributes) {
	DefaultLogger().Log(
	    level,
	    std::forward<Str>(message),
	    std::forward<Attributes>(attributes)...
	);
}

template <typename Str, typename... Attributes>
inline void Trace(Str &&message, Attributes &&...attributes) {
	DefaultLogger().Trace(
	    std::forward<Str>(message),
	    std::forward<Attributes>(attributes)...
	);
}

template <typename Str, typename... Attributes>
inline void Debug(Str &&message, Attributes &&...attributes) {
	DefaultLogger().Debug(
	    std::forward<Str>(message),
	    std::forward<Attributes>(attributes)...
	);
}

template <typename Str, typename... Attributes>
inline void Info(Str &&message, Attributes &&...attributes) {
	DefaultLogger().Info(
	    std::forward<Str>(message),
	    std::forward<Attributes>(attributes)...
	);
}

template <typename Str, typename... Attributes>
inline void Warn(Str &&message, Attributes &&...attributes) {
	DefaultLogger().Warn(
	    std::forward<Str>(message),
	    std::forward<Attributes>(attributes)...
	);
}

template <typename Str, typename... Attributes>
inline void Error(Str &&message, Attributes &&...attributes) {
	DefaultLogger().Error(
	    std::forward<Str>(message),
	    std::forward<Attributes>(attributes)...
	);
}

template <typename Str, typename... Attributes>
inline void Fatal(Str &&message, Attributes &&...attributes) {
	DefaultLogger().Fatal(
	    std::forward<Str>(message),
	    std::forward<Attributes>(attributes)...
	);
}

} // namespace slog

#include "slog++Impl.hpp"
