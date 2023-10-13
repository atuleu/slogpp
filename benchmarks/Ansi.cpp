#include "Ansi.hpp"

#include <cmath>

namespace ansi {
const static std::string esc = "\033[";

std::string SGR(Color fg, Color bg) noexcept {
	if (fg == Color::Unspecified && bg == Color::Unspecified) {
		return esc + "m";
	}
	static const char *fgCode[] = {
	    "30",
	    "31",
	    "32",
	    "33",
	    "34",
	    "35",
	    "36",
	    "37",
	    "39",
	    "90",
	    "91",
	    "92",
	    "93",
	    "94",
	    "95",
	    "96",
	    "97",
	};

	static const char *bgCode[] = {
	    "40",
	    "41",
	    "42",
	    "43",
	    "44",
	    "45",
	    "46",
	    "47",
	    "49",
	    "100",
	    "101",
	    "102",
	    "103",
	    "104",
	    "105",
	    "106",
	    "107",
	};

	if (bg == Color::Unspecified) {
		return esc + fgCode[int(fg)] + "m";
	}
	if (fg == Color::Unspecified) {
		return esc + bgCode[int(bg)] + "m";
	}
	return esc + fgCode[int(fg)] + ";" + bgCode[int(bg)] + "m";
}

std::string CursorUp(size_t lines) noexcept {
	if (lines == 0) {
		return "";
	} else if (lines == 1) {
		return esc + "A";
	}
	return esc + std::to_string(lines) + "A";
}

std::string InLineDelete(EraseMode mode) noexcept {
	return esc + std::to_string(int16_t(mode)) + "K";
}

int GetTTYWidth() {
	return 80;
}

std::string RenderProgressBar(double ratio, int width) noexcept {
	if (width <= 0) {
		return "";
	}
	size_t      pos = std::round(std::max(0.0, std::min(1.0, ratio)) * width);
	std::string res;
	res.reserve(2 * width + 8);
	for (size_t i = 0; i < pos; ++i) {
		res += "━";
	}
	res += SGR(Color::BrightBlack);
	for (int i = pos; i < width; ++i) {
		res += "━";
	}
	res += SGR();
	return res;
}

} // namespace ansi
