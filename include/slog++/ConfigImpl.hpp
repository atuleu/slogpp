#pragma once

#include "Config.hpp"
#include "Formatters.hpp"
#include "Level.hpp"
#include <type_traits>

#ifdef _WIN32
#include <io.h> // For _isatty() and _fileno() on Windows
#else
#include <unistd.h> // For isatty() and fileno() on POSIX systems
#endif

namespace slog {

inline Option<BaseSinkConfig> WithLocking() {
	return [](BaseSinkConfig &config) { config.withLocking = true; };
}

inline Option<BaseSinkConfig> WithAsync() {
	return [](BaseSinkConfig &config) { config.async = true; };
}

inline Option<BaseSinkConfig> WithFormat(OutputFormat format) {
	return [format](BaseSinkConfig &config) { config.format = format; };
}

inline Option<BaseSinkConfig> FromLevel(Level level) {
	return [level](BaseSinkConfig &config) {
		for (int i = int(level) + 1; i < NumLevels; ++i) {
			config.levels[i] = true;
		}
	};
}

template <typename... Levels>
inline Option<BaseSinkConfig> WithLevel(Levels... levels) {
	constexpr auto checkTypeIsLevel = [](auto &&level) {
		static_assert(
		    std::is_same_v<std::decay_t<decltype(level)>, Level>,
		    "all parameters should be of type Level"
		);
	};

	(checkTypeIsLevel(levels), ...);

	return [levels...](BaseSinkConfig &config) {
		auto setLevel = [&config](Level level) {
			size_t index = size_t(level) + 1;
			if (index >= NumLevels) {
				return;
			}
			config.levels[index] = true;
		};
		((setLevel(levels)), ...);
	};
}

inline Option<ProgramOutputSinkConfig> WithStdoutOutput() {
	return [](ProgramOutputSinkConfig &config) { config.toStdout = true; };
}

inline Option<ProgramOutputSinkConfig> WithForceColor() {
	return [](ProgramOutputSinkConfig &config) { config.forceColor = true; };
}

inline Option<ProgramOutputSinkConfig> WithDisabledColor() {
	return [](ProgramOutputSinkConfig &config) { config.disabledColor = true; };
}

template <typename... Configs>
inline Option<Config> WithProgramOutput(const Configs &...configs) {
	return [configs...](Config &config) {
		ProgramOutputSinkConfig sinkConfig;
		(configs(sinkConfig), ...);
		config.sinks.push_back(sinkConfig);
	};
}

template <typename... Configs>
inline Option<Config>
WithFileOutput(std::string filename, const Configs &...configs) {
	return [filename, configs...](Config &config) {
		FileSinkConfig sinkConfig;
		sinkConfig.filepath = filename;
		(configs(sinkConfig), ...);
		config.sinks.push_back(sinkConfig);
	};
}

inline Option<Config> WithThreadPoolSize(size_t size) {
	return [size](Config &config) { config.threadPoolSize = size; };
}

namespace details {

inline void Sanitize(Config &config) {
	// if no sink, add a default synchronous sink to STDERR, without locking
	// with Text format.
	if (config.sinks.empty()) {
		WithProgramOutput(WithFormat(OutputFormat::TEXT), FromLevel(Level::Info))(
		    config
		);
	}

	// if async sync are needed, set wanted threadpool size.
	bool hasAsync =
	    std::find_if(config.sinks.begin(), config.sinks.end(), [](auto &&sink) {
		    return std::visit(
		        [](auto &&s) -> bool { return s.async; },
		        std::forward<decltype(sink)>(sink)
		    );
	    }) != config.sinks.end();

	if (config.threadPoolSize == 0 && hasAsync == true) {
		config.threadPoolSize = 1;
	}
}

inline bool IsATTY(std::FILE *file) {
#ifdef _WIN32
	return _isatty(_fileno(file)) != 0;
#else
	return isatty(fileno(file));
#endif
};

} // namespace details

inline Formatter FileSinkConfig::Formatter() const noexcept {
	switch (format) {
	case OutputFormat::JSON:
		return &RecordToJSON;
	case OutputFormat::TEXT:
		return &RecordToRawText;
	}
	return &RecordToRawText;
}

inline Formatter ProgramOutputSinkConfig::Formatter() const noexcept {
	if (format == OutputFormat::JSON) {
		return &RecordToJSON;
	}

	if (disabledColor == true) {
		return &RecordToRawText;
	}

	FILE *outputStream = nullptr;
#ifdef _WIN32
	// On Windows, stdout/stderr are macros, so we use the underlying function
	outputStream = this->toStdout ? __acrt_iob_func(1) : __acrt_iob_func(2);
#else
	outputStream = this->toStdout ? stdout : stderr;
#endif

	if (forceColor == true || details::IsATTY(outputStream)) {
		return &RecordToANSIText;
	}
	return &RecordToRawText;
}

} // namespace slog
