#pragma once

#include <cstddef>

namespace slog {

enum class Level {
	Unknown  = -1,
	Trace    = 0,
	Debug    = 4,
	Info     = 8,
	Warn     = 12,
	Error    = 16,
	Critical = 20,
};

constexpr size_t NumLevels = size_t(Level::Critical) + 2;
} // namespace slog
