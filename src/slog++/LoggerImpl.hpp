#pragma once

#include "Logger.hpp"
#include <algorithm>
#include <string>
#include <sys/types.h>
#include <type_traits>

namespace slog {

template <typename Str, typename Value>
inline LogField<Str, Value> Field(const Str &key, Value &&value) {
	return LogField<Str, Value>{key, value};
}

template <typename... Fields>
inline Logger<Fields...>::Logger(const std::shared_ptr<Sink> &sink)
    : d_sink(sink) {}

template <typename... Fields>
template <typename... NewFields>
Logger<Fields..., NewFields...>
Logger<Fields...>::With(NewFields &&...fields) const {
	return Logger<Fields..., NewFields...>::Create(
	    d_sink,
	    std::tuple_cat(d_fields, std::make_tuple(std::forward(fields...))));
}

template <typename... Fields>
Logger<Fields..., LogField<const char *, const char *>>
Logger<Fields...>::WithError(const char *what) const {
	return With(Field("error", what));
}

template <typename... Fields>
template <typename Str, typename... NewFields>
void Logger<Fields...>::Log(Level level, const Str &msg,
                            NewFields &&...fields) const {}

template <typename Key, typename Value,
          typename std::enable_if<std::is_integral<Value>::value, bool>::value>
inline void ToJSON(const LogField<Key, Value> &field, std::string &buffer) {

	buffer += "\"";
	buffer += field.key;
	buffer += "\":" + std::to_string(field.value);
}

template <
    typename Key, typename Value,
    typename std::enable_if<std::is_floating_point<Value>::value, bool>::value>
inline void ToJSON(const LogField<Key, Value> &field, std::string &buffer) {
	buffer += "\"";
	buffer += field.key;
	buffer += "\":" + std::to_string(field.value);
}

template <typename Key, typename Value,
          typename std::enable_if<
              std::is_convertible<Value, std::string>::value, bool>::value>
inline void ToJSON(const LogField<Key, Value> &field, std::string &buffer) {
	std::string value = field.value;
	buffer += "\"";
	buffer += field.key;
	buffer += "\":\"";
	buffer += value;
	buffer += "\"";
}

} // namespace slog
