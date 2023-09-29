#pragma once

namespace slog {

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
	Logger(const std::shar);

	template <typename... Fields> const Logger &With(Fields &&...fields) const;

	template <typename Error> const Logger &WithError(const Error &error) const;

	template <typename Str> void Log(Level lvl, const Str &msg) const;

	template <typename Str> void Trace(const Str &msg) const {
		Log(Level::Trace, msg);
	}

	template <typename Str> void Debug(const Str &msg) const {
		Log(Level::Debug, msg);
	}

	template <typename Str> void Info(const Str &msg) const {
		Log(Level::Info, msg);
	};

	template <typename Str> void Warn(const Str &msg) const {
		Log(Level::Warning, msg);
	}

	template <typename Str> void Error(const Str &msg) const {
		Log(Level::Error, msg);
	}

	template <typename Str> void Fatal(const Str &msg) const {
		Log(Level::Fatal, msg);
	}
};

} // namespace slog
