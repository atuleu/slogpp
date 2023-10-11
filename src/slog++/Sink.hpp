#pragma once

#include "Level.hpp"
#include <array>

namespace slog {
class RecordBase;
class BaseSinkConfig;

class Sink {
public:
	Sink(const BaseSinkConfig &config);

	virtual ~Sink() {}

	bool Enabled(Level lvl) const noexcept = 0;

	virtual void Log(const RecordBase &record) = 0;

private:
	std::array<bool, NumLevels> d_levels;
};

class ThreadSafeSink : public Sink {};

}; // namespace slog
