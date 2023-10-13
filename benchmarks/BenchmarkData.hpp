#pragma once

#include <string>

#include "Types.hpp"

struct BenchmarkData {

	int         code;
	double      value;
	std::string domain;
	Duration    duration;
	Timepoint   time;

	struct Request {
		std::string url;
		int         status;

		// A constructor that draws random data;
		Request();
	};

	Request request;

	// A constructor that draw random data
	BenchmarkData();
};
