#pragma once

#include <cstdint>
#include <string>

namespace ansi {

enum class Color {
	Black = 0,
	Red,
	Green,
	Yellow,
	Blue,
	Magenta,
	Cyan,
	White,
	Reset,
	BrightBlack,
	BrightRed,
	BrightGreen,
	BrightYellow,
	BrightBlue,
	BrightMagenta,
	BrightCyan,
	BrightWhite,
	Unspecified,
};

std::string
SGR(Color fg = Color::Unspecified, Color bg = Color::Unspecified) noexcept;

std::string CursorUp(size_t lines = 1) noexcept;

enum class EraseMode {
	ToEnd   = 0,
	ToBegin = 1,
	All     = 2,
};

std::string InLineDelete(EraseMode mode = EraseMode::ToEnd) noexcept;

std::string RenderProgressBar(double ratio, int width) noexcept;

int GetTTYWidth();
} // namespace ansi
