#pragma once

#include <gmock/gmock.h>

#include "Sink.hpp"

namespace slog {

class MockSink : public Sink {
public:
	MOCK_METHOD(bool, Enabled, (Level lvl), (const override));
	MOCK_METHOD(void, Log, (const Record &record), (override));
};

} // namespace slog
