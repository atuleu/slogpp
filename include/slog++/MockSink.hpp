#pragma once

#include <gmock/gmock.h>

#include "Sink.hpp"

namespace slog {

class MockSink : public Sink {
public:
	MOCK_METHOD(bool, AllocateOnStack, (), (const, noexcept, override));
	MOCK_METHOD(bool, Enabled, (Level lvl), (const, noexcept, override));
	MOCK_METHOD(void, Log, (Sink::RecordVariant &&), (override));
};

} // namespace slog
