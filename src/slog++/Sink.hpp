#pragma once

#include "Level.hpp"
#include <array>

namespace slog {
class Record;
class BaseSinkConfig;

class Sink {
public:
	virtual ~Sink() = default;

	virtual bool Enabled(Level lvl) const noexcept = 0;

	virtual void Log(const Record &record) = 0;
};

}; // namespace slog
