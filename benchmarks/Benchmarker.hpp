#pragma once

#include <algorithm>
#include <array>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <random>
#include <string>
#include <tuple>
#include <type_traits>
#include <vector>

#include "Ansi.hpp"
#include "Types.hpp"

struct BenchmarkResult {
	BenchmarkResult(std::vector<Duration> &&durations);
	std::tuple<double, std::string> Cast(const Duration &d) const noexcept;

	std::vector<Duration> durations;
	Duration              total;
	Duration              average;
	Duration              sigma;
	Duration              minimum;
	Duration              maximum;
	std::string           units;
	double                ratio;
};

std::ostream &operator<<(std::ostream &out, BenchmarkResult &result);

inline std::string NameEllipsis(const std::string &str, size_t columns) {
	size_t nameSize = std::max(0.0, std::round(columns * 0.25));
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

	Duration target         = std::chrono::seconds{2};
	Duration maxWarmingUp   = std::chrono::milliseconds{500};
	size_t   warmup         = 20;
	size_t   minIterations  = 50;
	size_t   maxIteration   = 100000;

	bool     display = true;
	int      columns = 80;
	Duration refresh = std::chrono::milliseconds(100);

	template <typename F>
	inline BenchmarkResult Benchmark(const std::string &name, F &&fn) const {

		State state{NameEllipsis(name, columns)};

		std::vector<Duration> results{};

		while (ShouldStop(state) == false) {
			auto start = Timepoint::clock::now();
			std::forward<F>(fn)(data[state.iterations % N]);
			auto end = Timepoint::clock::now();

			results.push_back(end - start);
			IncrementState(state, results.back());
			DisplayState(state, end, false);
		}

		DisplayState(state, Timepoint::clock::now(), true);
		return results;
	}

	inline Benchmarker()
	    : columns{std::min(100, ansi::GetTTYWidth())} {}

private:
	struct State {
		std::string name;
		Timepoint   start;
		Timepoint   lastDisplay;
		size_t      iterations      = 0;
		size_t      nbDisplay       = 0;
		size_t      stopAtIteration = 0;
		Duration    totalDuration   = std::chrono::nanoseconds{0};

		inline State(std::string name) noexcept
		    : name{name}
		    , start{Timepoint::clock::now()} {}

		inline bool WarmingUp() const noexcept {
			return stopAtIteration == 0;
		}
	};

	inline void
	IncrementState(State &state, const Duration &last) const noexcept {
		state.totalDuration += last;
		state.iterations += 1;

		if (!state.WarmingUp()) {
			// warmed up
			return;
		}

		if (state.iterations < warmup && state.totalDuration < maxWarmingUp) {
			// warming up
			return;
		}

		// estimate to end at target
		state.stopAtIteration =
		    std::round(double(target.count()) / state.totalDuration.count());

		// makes iterations into bounds
		state.stopAtIteration =
		    std::clamp(state.stopAtIteration, minIterations, maxIteration);
	}

	inline bool ShouldStop(const State &state) const noexcept {
		return !state.WarmingUp() && state.iterations >= minIterations &&
		       (state.iterations >= state.stopAtIteration ||
		        state.totalDuration >= 2 * target);
	}

	inline double Ratio(const State &state) const noexcept {
		if (state.WarmingUp()) {
			return std::max(
			    state.iterations / double(warmup),
			    state.totalDuration.count() / double(maxWarmingUp.count())
			);
		}

		return std::max(
		    state.iterations / double(state.stopAtIteration),
		    state.totalDuration.count() / (2.0 * target.count())
		);
	}

	inline void DisplayState(State &state, const Timepoint &now, bool terminate)
	    const noexcept {
		bool displayDisabled       = !display || columns <= 0;
		bool refreshFrequencyGuard = (now - state.lastDisplay) < refresh;

		if (displayDisabled || refreshFrequencyGuard && !terminate) {
			// do not refresh
			return;
		}
		state.lastDisplay = now;
		state.nbDisplay++;

		if (state.nbDisplay > 1) {
			std::cout << ansi::CursorUp()
			          << ansi::InLineDelete(ansi::EraseMode::All);
		}
		const static std::string spin[] = {"⠷", "⠯", "⠟", "⠻", "⠽", "⠾"};

		auto spinner = terminate ? "⠿" : spin[state.nbDisplay % 6];

		std::cout << state.name << " ";

		if (state.WarmingUp() && !terminate) {
			std::cout << std::setw(13) << "warming up";
		} else {
			std::cout << std::setw(6) << state.iterations << "/" << std::setw(6)
			          << state.stopAtIteration;
		}

		auto ratio = Ratio(state);
		auto color = ansi::Color::Cyan;
		if (terminate) {
			color = ansi::Color::Green;
			ratio = 1.0;
		} else if (state.WarmingUp()) {
			color = ansi::Color::Yellow;
		}

		std::cout << ansi::SGR(color) << " " << spinner << " ";

		size_t pbWidth = columns - state.name.size() - 14 - 3 - 5;

		const auto ellapsed =
		    std::chrono::duration_cast<std::chrono::duration<double>>(
		        state.totalDuration
		    );

		std::cout << ansi::RenderProgressBar(ratio, pbWidth) << " "
		          << std::fixed << std::setprecision(1) << ellapsed.count()
		          << "s" << std::endl;
	}
};
