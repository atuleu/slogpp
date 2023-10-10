#pragma once

namespace slog {

enum class Level {
	Trace    = -8,
	Debug    = -4,
	Info     = 0,
	Warn     = 4,
	Error    = 8,
	Critical = 12,
};
}
