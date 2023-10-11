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

	bool                        withLocking = false;
	bool                        async       = false;
	OutputFormat                format      = OutputFormat::JSON;
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

struct Config {
	std::vector<SinkConfig> sinks;
};

template <typename T> using Option = std::function<void(T &)>;

Option<BaseSinkConfig> WithLocking();

Option<BaseSinkConfig> WithAsync();

Option<BaseSinkConfig> WithFormat(OutputFormat format);

Option<BaseSinkConfig> FromLevel(Level level);

template <typename... Levels>
Option<BaseSinkConfig> WithLevel(Levels... levels);

Option<ProgramOutputSinkConfig> WithStdoutOutput();

Option<ProgramOutputSinkConfig> WithForceColor();

Option<ProgramOutputSinkConfig> WithDisabledColor();

template <typename... Configs>
Option<Config> WithProgramOutput(const Configs &...configs);

template <typename... Configs>
Option<Config> WithFileOutput(std::string filename, const Configs &...configs);

} // namespace slog

#include "ConfigImpl.hpp"
