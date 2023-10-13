#pragma once

#include <chrono>

using Duration  = std::chrono::nanoseconds;
using Timepoint = std::chrono::time_point<std::chrono::system_clock, Duration>;
