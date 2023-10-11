#pragma once

#include <functional>
#include <string>
#include <variant>
#include <vector>

#include "Level.hpp"

namespace slog {

enum class OutputFormat {
	JSON = 0,
	TEXT = 1,
};

struct BaseSinkConfig {

	bool                        threadSafe    = false;
	bool                        multiThreaded = false;
	OutputFormat                format        = OutputFormat::JSON;
	std::array<bool, NumLevels> levels;

	BaseSinkConfig() {
		levels.fill(false);
	}
};

struct ProgramOutputSinkConfig : BaseSinkConfig {
	bool stdout        = false;
	bool disabledColor = false;
	bool forceColor    = false;
};

struct FileSinkConfig : BaseSinkConfig {
	std::string filepath;
};

using SinkConfig = std::variant<ProgramOutputSinkConfig, FileSinkConfig>;

// TODO: OpenTelemetry sink;
struct Config {
	std::vector<SinkConfig> sinks;
};

template <typename T> using Option = std::function<void(T &)>;

inline Option<BaseSinkConfig> WithThreadSafeLock() {
	return [](BaseSinkConfig &config) { config.threadSafe = true; };
}

inline Option<BaseSinkConfig> WithMultiThread() {
	return [](BaseSinkConfig &config) { config.multiThreaded = true; };
}

inline Option<BaseSinkConfig> WithFormat(OutputFormat format) {
	return [format](BaseSinkConfig &config) { config.format = format; };
}

inline Option<BaseSinkConfig> WithLevelFrom(Level level) {
	return [level](BaseSinkConfig &config) {
		for (int i = int(level) + 1; i < int(Level::Critical) + 2; ++i) {
			config.levels.levels[i] = true;
		}
	};
}

inline Option<BaseSinkConfig> WithLevel(Level level) {
	return [level](BaseSinkConfig &config) {
		if (int(level) < Level::Critical + 2) {
			config.levels.levels[int(level) + 1] = true;
		}
	};
}

inline Option<ProgramOutputSinkConfig> WithStdoutOuput() {
	return [](ProgramOutputSinkConfig &config) { config.stdout = true; };
}

inline Option<ProgramOutputSinkConfig> WithForceColor() {
	return [](ProgramOutputSinkConfig &config) { config.forceColor = true; };
}

inline Option<ProgramOutputSinkConfig> WithDisabledColor() {
	return [](ProgramOutputSinkConfig &config) { config.disabledColor = true; };
}

template <typename... Configs>
inline Option<Config> WithProgramOutput(Configs &&...configs) {
	return [configs...](Config &config) {
		ProgramOutputSinkConfig sinkConfig;
		((std::forward<Configs>(configs)(sinkConfig)), ...);
		config.sinks.push_back(sinkConfig);
	};
}

template <typename... Configs>
inline Option<Config>
WithFileOutput(std::string filename, Configs &&...configs) {
	return [filename, configs...](Config &config) {
		FileSinkConfig sinkConfig;
		sinkConfig.filepath = filename;
		((std::forward<Configs>(configs)(sinkConfig)), ...);
		config.sinks.push_back(sinkConfig);
	};
}

} // namespace slog
