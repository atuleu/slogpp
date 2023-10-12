#pragma once

#include "Attribute.hpp"
#include "Level.hpp"

#include <memory>
#include <type_traits>

namespace slog {
class Sink;

template <size_t N> class Logger {
public:
	template <size_t M> friend class Logger;

	Logger(std::shared_ptr<Sink> sink) noexcept;

	void SetSink(std::shared_ptr<Sink> sink) {
		d_sink = std::move(sink);
	}

	template <typename... Attributes>
	Logger<N + sizeof...(Attributes)> With(Attributes &&...attributes
	) const noexcept;

	template <typename Str, typename... Attributes>
	void Log(Level level, Str &&msg, Attributes &&...attributes) const;

	template <typename Str, typename... Attributes>
	inline void Trace(Str &&msg, Attributes &&...attributes) const {
		Log(Level::Trace,
		    std::forward<Str>(msg),
		    std::forward<Attributes>(attributes)...);
	};

	template <typename Str, typename... Attributes>
	inline void Debug(Str &&msg, Attributes &&...attributes) const {
		Log(Level::Debug,
		    std::forward<Str>(msg),
		    std::forward<Attributes>(attributes)...);
	};

	template <typename Str, typename... Attributes>
	inline void Info(Str &&msg, Attributes &&...attributes) const {
		Log(Level::Info,
		    std::forward<Str>(msg),
		    std::forward<Attributes>(attributes)...);
	};

	template <typename Str, typename... Attributes>
	inline void Warn(Str &&msg, Attributes &&...attributes) const {
		Log(Level::Warn,
		    std::forward<Str>(msg),
		    std::forward<Attributes>(attributes)...);
	};

	template <typename Str, typename... Attributes>
	inline void Error(Str &&msg, Attributes &&...attributes) const {
		Log(Level::Error,
		    std::forward<Str>(msg),
		    std::forward<Attributes>(attributes)...);
	};

	template <typename Str, typename... Attributes>
	inline void Fatal(Str &&msg, Attributes &&...attributes) const {
		Log(Level::Fatal,
		    std::forward<Str>(msg),
		    std::forward<Attributes>(attributes)...);
	};

private:
	std::shared_ptr<Sink>    d_sink;
	std::array<Attribute, N> d_attributes;
};

} // namespace slog

#include "LoggerImpl.hpp"
