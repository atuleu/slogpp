#pragma once

#include "Attribute.hpp"
#include "Level.hpp"

namespace slog {

struct Record {
	TimeT                  timestamp;
	Level                  level;
	std::string            message;
	std::vector<Attribute> attributes;

	Record(Level lvl, std::string &&message, size_t capacity = 0);

	template <typename Iter>
	void PushAttributes(const Iter &begin, const Iter &end);
	void PushAttribute(Attribute &&attr);
};

} // namespace slog

#include "RecordImpl.hpp"
