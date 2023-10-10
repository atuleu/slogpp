#pragma once

#include "Level.hpp"
#include "Record.hpp"

namespace slog {

class Sink {
public:
	virtual ~Sink() {}

	virtual bool Enabled(Level lvl) const = 0;

	virtual void Log(const RecordBase &record) = 0;
};

}; // namespace slog
