#pragma once

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

template <typename Key, typename Value> struct LogField {
	Key   key;
	Value value;
};

template <
    typename Key, typename Value,
    typename std::enable_if<std::is_integral<Value>::value, bool>::value = true>
void ToJSON(const LogField<Key, Value> &field, std::string &buffer);

template <typename Key, typename Value,
          typename std::enable_if<std::is_floating_point<Value>::value,
                                  bool>::value = true>
void ToJSON(const LogField<Key, Value> &field, std::string &buffer);

template <
    typename Key, typename Value,
    typename std::enable_if<std::is_convertible<Value, std::string>::value,
                            bool>::value = true>
void ToJSON(const LogField<Key, Value> &field, std::string &buffer);

template <typename Str, typename Value>
LogField<Str, Value> Field(const Str &key, Value &&value);

template <typename... Fields> class Logger {
public:
	Logger(const std::shared_ptr<Sink> &sink);

	template <typename... NewFields>
	Logger<Fields..., NewFields...> With(NewFields &&...fields) const;

	Logger<Fields..., LogField<const char *, const char *>>
	WithError(const char *what) const;

	template <typename Str, typename... NewFields>
	void Log(Level level, const Str &msg, NewFields &&...fields) const;

	template <typename Str, typename... NewFields>
	inline void Trace(const Str &msg, NewFields &&...fields) const {
		Log(Level::Trace, msg, std::forward(fields...));
	};

	template <typename Str, typename... NewFields>
	inline void Debug(const Str &msg, NewFields &&...fields) const {
		Log(Level::Debug, msg, std::forward(fields...));
	};

	template <typename Str, typename... NewFields>
	inline void Info(const Str &msg, NewFields &&...fields) const {
		Log(Level::Info, msg, std::forward(fields...));
	};

	template <typename Str, typename... NewFields>
	inline void Warn(const Str &msg, NewFields &&...fields) const {
		Log(Level::Warn, msg, std::forward(fields...));
	};

	template <typename Str, typename... NewFields>
	inline void Error(const Str &msg, NewFields &&...fields) const {
		Log(Level::Error, msg, std::forward(fields...));
	};

	template <typename Str, typename... NewFields>
	inline void Critical(const Str &msg, NewFields &&...fields) const {
		Log(Level::Critical, msg, std::forward(fields...));
	};

private:
	static Logger Create(const std::shared_ptr<Sink> &sink,
	                     std::tuple<Fields...>      &&fields);

	std::shared_ptr<Sink> d_sink;
	std::tuple<Fields...> d_fields;
};

} // namespace slog
