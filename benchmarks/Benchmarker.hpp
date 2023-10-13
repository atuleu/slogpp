#pragma once

#include "Types.hpp"
#include <array>
#include <chrono>
#include <iostream>
#include <iterator>
#include <random>
#include <string>
#include <type_traits>
#include <vector>

struct BenchmarkResult {
	std::vector<Duration> durations;
};

size_t GetTTYWidth();

std::string NameEllipsis(const std::string &str, size_t columns);

template <typename T, size_t N, std::enable_if_t<N >= 1> * = nullptr>
struct Benchmarker {
	std::array<T, N> data;

	Duration        target  = std::chrono::seconds{2};
	size_t          minIterations = 10;
	bool            display       = true;
	size_t          columns       = 80;
	Duration        refresh       = std::chrono::milliseconds(50);

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
	    : columns{GetTTYWidth()} {}

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
		       (now - state.start).count() >= target;
	}

	void DisplayState(
	    const State &state, const Timepoint &now, bool terminate
	) noexcept {
		if (!display || columns <= 0 || (now - last) < refresh) {
			return;
		}
		last = now;
		++nbDisplay;

		if (nbDisplay > 1) {
			std::cout << "\033[A\033[2K";
		}
		const static std::string spin[] = {"⠷", "⠯", "⠟", "⠻", "⠽", "⠾"};

		std::cout << state.name << " ";
		if (terminate) {
			std::cout << "⠿";
		} else {
			std::cout << "\033]36mspin[nbDisplay % 6]\033[m ";
		}
	}
};
