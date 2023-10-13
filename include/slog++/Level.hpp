#pragma once

#include <cstddef>
#include <type_traits>

namespace slog {

enum class Level {
	Unknown = -1,
	Trace   = 0,
	Debug   = 4,
	Info    = 8,
	Warn    = 12,
	Error   = 16,
	Fatal   = 20,
};

template <
    Level  L,
    size_t N,
    std::enable_if_t<
        N >= 0 && N <= 3 //
        && L != Level::Unknown && L != Level::Fatal> * = nullptr>
inline const Level SubLevel = static_cast<Level>(static_cast<size_t>(L) + N);

constexpr size_t NumLevels = size_t(Level::Fatal) + 2;
} // namespace slog
