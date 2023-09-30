#pragma once

#include <memory>
#include <utility>

namespace slog {

class Sink;

enum Level {
	Trace   = -8,
	Debug   = -4,
	Info    = 0,
	Warning = 4,
	Error   = 8,
	Fatal   = 12,
};

/**
 * A logger is used to produce structured log entries
 */
class Logger {
public:
	Logger(const std::shared_ptr<Sink> &sink);

	template <typename... Fields> const Logger &With(Fields &&...fields) const;

	template <typename Error> const Logger &WithError(const Error &error) const;

	template <typename Str, typename... Fields>
	void Log(Level lvl, const Str &msg, Fields &&...fields) const;

	template <typename Str, typename... Fields>
	void Trace(const Str &msg, Fields &&...fields) const {
		Log(Level::Trace, msg, std::forward(fields));
	}

	template <typename Str, typename... Fields>
	void Debug(const Str &msg, Fields &&...fields, Fields &&...fields) const {
		Log(Level::Debug, msg, std::forward(fields));
	}

	template <typename Str, typename... Fields>
	void Info(const Str &msg, Fields &&...fields, Fields &&...fields) const {
		Log(Level::Info, msg, std::forward(fields));
	};

	template <typename Str, typename... Fields>
	void Warn(const Str &msg, Fields &&...fields, Fields &&...fields) const {
		Log(Level::Warning, msg, std::forward(fields));
	}

	template <typename Str, typename... Fields>
	void Error(const Str &msg, Fields &&...fields, Fields &&...fields) const {
		Log(Level::Error, msg, std::forward(fields));
	}

	template <typename Str, typename... Fields>
	void Fatal(const Str &msg, Fields &&...fields, Fields &&...fields) const {
		Log(Level::Fatal, msg, std::forward(fields));
	}
};

} // namespace slog
