#pragma once

#include "Record.hpp"
#include <algorithm>
#include <iterator>

namespace slog {

inline Record::Record(Level lvl, std::string &&msg, size_t capacity)
    : timestamp(std::chrono::time_point_cast<DurationT>(
          std::chrono::system_clock::now()
      ))
    , level(lvl)
    , message(msg) {
	attributes.reserve(capacity);
}

template <typename Iter>
inline void Record::PushAttributes(const Iter &begin, const Iter &end) {
	std::copy(begin, end, std::back_inserter(attributes));
}

inline void Record::PushAttribute(Attribute &&attr) {
	attributes.push_back(std::move(attr));
}

} // namespace slog
