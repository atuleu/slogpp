#pragma once

#include "Config.hpp"
#include "FileSink.hpp"
#include "slog++.hpp"

#include <stdexcept>

namespace slog {

namespace details {

inline std::shared_ptr<slog::Sink> BuildSink(const SinkConfig &config) {
	using Ptr = std::shared_ptr<slog::Sink>;
	return std::visit(
	    [](auto &&config) -> std::shared_ptr<slog::Sink> {
		    using T = std::decay_t<decltype(config)>;
		    if constexpr (std::is_same_v<T, ProgramOutputSinkConfig> || std::is_same_v<T, FileSinkConfig>) {
			    if (config.async) {
				    if (config.withLocking) {
					    return Ptr(new FileSink<AsyncMtSafe>(config));
				    } else {
					    return Ptr(new FileSink<Async>(config));
				    }
			    } else {
				    if (config.withLocking) {
					    return Ptr(new FileSink<MTSafe>(config));
				    } else {
					    return Ptr(new FileSink<Unsafe>(config));
				    }
			    }
		    } else {
			    static_assert(always_false_v<T>, "non-exhaustive visitor");
		    }
	    },
	    config
	);
}

} // namespace details

template <typename... Options>
inline std::shared_ptr<Sink> BuildSink(Options &&...options) {
	Config config;
	(std::forward<Options>(options)(config), ...);
	details::Sanitize(config);

	if (config.sinks.size() == 1) {
		return details::BuildSink(config.sinks.front());
	}

	throw std::logic_error("Multiple sink per logger isn't supported yet");

	std::vector<std::shared_ptr<Sink>> sinks;
	sinks.reserve(config.sinks.size());
	for (const auto &sinkConfig : config.sinks) {
		sinks.push_back(details::BuildSink(sinkConfig));
	}

	// TODO: need implementation of a multisink
	return nullptr;
}

} // namespace slog
