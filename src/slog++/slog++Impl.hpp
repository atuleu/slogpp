#pragma once

#include "slog++.hpp"

#include <stdexcept>

namespace slog {
class Sink;

namespace details {

inline std::shared_ptr<slog::Sink> BuildSink(const SinkConfig &config) {
	throw std::logic_error("Building sink from config is not yet supported");
	return nullptr;
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
