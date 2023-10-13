#pragma once

#include "Level.hpp"

#include <memory>
#include <variant>

namespace slog {
class Record;
class BaseSinkConfig;

class Sink {
public:
	using RecordVariant =
	    std::variant<const Record *, std::unique_ptr<const Record>>;

	virtual ~Sink() = default;

	virtual bool AllocateOnStack() const noexcept = 0;

	virtual bool Enabled(Level lvl) const noexcept = 0;

	virtual void Log(RecordVariant &&record) = 0;
};

}; // namespace slog
