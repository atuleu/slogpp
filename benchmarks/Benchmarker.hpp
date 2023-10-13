#pragma once

#include <array>
#include <chrono>
#include <iostream>
#include <iterator>
#include <random>
#include <string>
#include <type_traits>
#include <vector>

#include "Ansi.hpp"
#include "Types.hpp"

struct BenchmarkResult {
	std::vector<Duration> durations;
};

inline std::string NameEllipsis(const std::string &str, size_t columns) {
	size_t nameSize = std::max(0.0, std::round(columns * 0.3));
	if (nameSize < 3) {
		return str;
	}
	if (str.size() > nameSize) {
		return str.substr(0, nameSize - 3) + "...";
	}

	return str + std::string(nameSize - str.size(), ' ');
}

enum class AnsiColor {
	Reset,
	Black,
	White,
	Yellow,
	Green,
	BrightBlack,
	BrightWight,
};

template <typename T, size_t N, std::enable_if_t<N >= 1> * = nullptr>
struct Benchmarker {
	std::array<T, N> data;

	Duration target        = std::chrono::seconds{2};
	size_t   minIterations = 10;
	bool     display       = true;
	int      columns       = 80;
	Duration refresh       = std::chrono::milliseconds(50);

	template <typename F>
	inline BenchmarkResult Benchmark(const std::string &name, F &&fn) {

		State state{NameEllipsis(name, columns)};

		BenchmarkResult result{};
		size_t          i = -1;

		Reset();

		for (auto end = Timepoint::clock::now();
		     ShouldStop(state, end) == false;
		     state.iterations++) {
			auto start = Timepoint::clock::now();
			std::forward<F>(fn)(data[state.iterations % N]);
			end = Timepoint::clock::now();

			result.durations.push_back(end - start);
			DisplayState(state, end, false);
		}

		DisplayState(state, Timepoint::clock::now(), true);
		return result;
	}

	inline Benchmarker()
	    : columns{ansi::GetTTYWidth()} {}

private:
	struct State {
		std::string name;
		Timepoint   start;
		size_t      iterations = 0;

		State(std::string name)
		    : name{name}
		    , start{Timepoint::clock::now()} {}
	};

	Timepoint last;
	size_t    nbDisplay;

	inline void Reset() {
		nbDisplay = 0;
		last      = Timepoint::clock::now() - refresh;
	}

	inline bool
	ShouldStop(const State &state, const Timepoint &now) const noexcept {
		return state.iterations >= minIterations &&
		       (now - state.start) >= target;
	}

	void DisplayState(
	    const State &state, const Timepoint &now, bool terminate
	) noexcept {
		if (!display || columns <= 0 ||
		    ((now - last) < refresh) && !terminate) {
			return;
		}
		last = now;
		++nbDisplay;

		if (nbDisplay > 1) {
			std::cout << ansi::CursorUp()
			          << ansi::InLineDelete(ansi::EraseMode::All);
		}
		const static std::string spin[] = {"⠷", "⠯", "⠟", "⠻", "⠽", "⠾"};

		bool isWarmingUp = state.iterations < minIterations;
		auto color       = ansi::Color::Cyan;
		auto ratio       = (now - state.start).count() / double(target.count());

		if (isWarmingUp == true) {
			color = ansi::Color::Yellow;
			ratio = double(state.iterations + 1) / minIterations;
		}
		if (terminate) {
			color = ansi::Color::Green;
			ratio = 1.0;
		}

		std::cout << state.name << " " << ansi::SGR(color);
		if (terminate) {
			std::cout << "⠿ ";
		} else {
			std::cout << spin[nbDisplay % 6] << " ";
		}

		std::cout
		    << ansi::RenderProgressBar(ratio, columns - state.name.size() - 3)
		    << std::endl;
	}
};
