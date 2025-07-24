#pragma once

#include <algorithm>
#include <initializer_list>
#include <memory>
#include <vector>

#include "Level.hpp"
#include "Record.hpp"
#include "Sink.hpp"

namespace slog {

namespace details {

class MultiSink : public slog::Sink {
public:
	typedef std::shared_ptr<Sink> SinkPtr;

	MultiSink(std::vector<SinkPtr> &&sinks)
	    : d_sinks{std::move(sinks)} {
		update();
	}

	inline bool AllocateOnStack() const noexcept override {
		return d_allocateOnStack;
	}

	bool Enabled(Level lvl) const noexcept override {
		return lvl >= d_from;
	}

	void From(Level lvl) noexcept override {
		for (auto &s : d_sinks) {
			s->From(lvl);
		}
	}

	void Set(Level lvl, bool enabled) noexcept override {
		for (auto &s : d_sinks) {
			s->Set(lvl, enabled);
		}
	}

	void Log(RecordVariant &&record) override {
		auto level = std::visit(
		    [](const auto &arg) -> Level { return arg->level; },
		    record
		);
		auto cloned = cloneRecord(std::move(record));

		for (auto &sink : d_sinks) {
			if (sink->Enabled(level) == false) {
				continue;
			}
			sink->Log(std::visit(
			    [](const auto &arg) -> RecordVariant { return arg; },
			    cloned
			));
		}
	}

private:
	using UniquePtr = std::unique_ptr<const slog::Record>;
	using SharedPtr = std::shared_ptr<const slog::Record>;
	using Ptr       = const slog::Record *;

	static std::variant<Ptr, SharedPtr> cloneRecord(RecordVariant &&record) {
		return std::visit(
		    [](auto &&arg) -> std::variant<Ptr, SharedPtr> {
			    using T = std::decay_t<decltype(arg)>;
			    if constexpr (std::is_same_v<T, UniquePtr>) {
				    return SharedPtr(std::move(arg));
			    } else if constexpr (std::is_same_v<T, Ptr>) {
				    return arg;
			    } else if constexpr (std::is_same_v<T, SharedPtr>) {
				    return arg;
			    }
		    },
		    std::move(record)
		);
	}

	void update() {
		d_sinks.erase(
		    std::remove(d_sinks.begin(), d_sinks.end(), nullptr),
		    d_sinks.end()
		);

		d_allocateOnStack = true;
		d_from            = Level::Fatal;
		for (const auto &s : d_sinks) {
			d_allocateOnStack = d_allocateOnStack && s->AllocateOnStack();
			Level from        = Level::Fatal;
			for (size_t i = 0; i < NumLevels; ++i) {
				if (s->Enabled(Level(i))) {
					from = Level(i);
					break;
				}
			}
			d_from = std::min(d_from, from);
		}
	}

	std::vector<std::shared_ptr<Sink>> d_sinks;
	Level                              d_from;
	bool                               d_allocateOnStack;
};

} // namespace details

template <typename... Sinks>
static std::shared_ptr<Sink> TeeSink(Sinks &&...sinks) {
	std::vector<std::shared_ptr<Sink>> _sinks = {std::forward<Sinks>(sinks)...};
	return std::make_shared<details::MultiSink>(std::move(_sinks));
}
} // namespace slog
