#include "Config.hpp"
#include "Level.hpp"

#include "gmock/gmock.h"
#include <gtest/gtest.h>
#include <initializer_list>
#include <variant>

namespace slog {

TEST(BaseSinkConfig, Default) {
	BaseSinkConfig defaultValue{};
	EXPECT_FALSE(defaultValue.withLocking);
	EXPECT_FALSE(defaultValue.async);
	EXPECT_EQ(defaultValue.format, OutputFormat::JSON);
	for (auto enabled : defaultValue.levels) {
		EXPECT_FALSE(enabled);
	}
}

BaseSinkConfig buildBaseSinkConfig(
    bool                                withLocking,
    bool                                async,
    OutputFormat                        format,
    const std::initializer_list<Level> &enabledLevels
) {
	BaseSinkConfig config{};
	config.withLocking = withLocking;
	config.async       = async;
	config.format      = format;
	for (auto level : enabledLevels) {
		size_t index = int(level) + 1;
		if (index < 1 && index >= NumLevels) {
			continue;
		}
		config.levels[size_t(level) + 1] = true;
	}
	return config;
}

auto BasicSinkConfigEqual(const BaseSinkConfig &config) {
	using namespace ::testing;
	return AllOf(
	    Field(
	        "withLocking",
	        &BaseSinkConfig::withLocking,
	        Eq(config.withLocking)
	    ),
	    Field("async", &BaseSinkConfig::async, Eq(config.async)),
	    Field("format", &BaseSinkConfig::format, Eq(config.format)),
	    Field(
	        "levels",
	        &BaseSinkConfig::levels,
	        ElementsAreArray(config.levels)
	    )
	);
};

TEST(BaseSinkConfig, Options) {
	struct TestData {
		std::string            Name;
		Option<BaseSinkConfig> Opt;
		BaseSinkConfig         Expected;
	};

	std::vector<TestData> testdata = {
	    {
	        "WithLocking",
	        WithLocking(),
	        buildBaseSinkConfig(true, false, OutputFormat::JSON, {}),
	    },
	    {
	        "WithAsync",
	        WithAsync(),
	        buildBaseSinkConfig(false, true, OutputFormat::JSON, {}),
	    },
	    {
	        "WithFormat",
	        WithFormat(OutputFormat::TEXT),
	        buildBaseSinkConfig(false, false, OutputFormat::TEXT, {}),
	    },
	    {
	        "WithLevel",
	        WithLevel(Level::Debug, Level::Error, SubLevel<Level::Error, 2>),
	        buildBaseSinkConfig(
	            false,
	            false,
	            OutputFormat::JSON,
	            {Level::Debug, Level::Error, SubLevel<Level::Error, 2>}
	        ),
	    },
	    {
	        "FromLevel",
	        FromLevel(Level::Warn),
	        buildBaseSinkConfig(
	            false,
	            false,
	            OutputFormat::JSON,
	            {
	                Level::Warn,
	                SubLevel<Level::Warn, 1>,
	                SubLevel<Level::Warn, 2>,
	                SubLevel<Level::Warn, 3>,
	                Level::Error,
	                SubLevel<Level::Error, 1>,
	                SubLevel<Level::Error, 2>,
	                SubLevel<Level::Error, 3>,
	                Level::Critical,
	            }
	        ),
	    },

	};
	using namespace ::testing;

	for (const auto &data : testdata) {
		SCOPED_TRACE(data.Name);
		BaseSinkConfig config{};
		data.Opt(config);
		EXPECT_THAT(config, BasicSinkConfigEqual(data.Expected));
	}
}

TEST(ProgramOutputSinkConfig, Default) {
	ProgramOutputSinkConfig defaultValue{};
	EXPECT_FALSE(defaultValue.stdout);
	EXPECT_FALSE(defaultValue.disabledColor);
	EXPECT_FALSE(defaultValue.forceColor);
}

ProgramOutputSinkConfig
buildProgramOutputSinkConfig(bool stdout, bool disabledColor, bool forceColor) {
	ProgramOutputSinkConfig config;
	config.stdout        = stdout;
	config.disabledColor = disabledColor;
	config.forceColor    = forceColor;
	return config;
}

auto ProgramOutputSinkConfigAreEqual(const ProgramOutputSinkConfig &config) {
	using namespace ::testing;
	return AllOf(
	    BasicSinkConfigEqual(config),
	    Field("stdout", &ProgramOutputSinkConfig::stdout, Eq(config.stdout)),
	    Field(
	        "disabledColor",
	        &ProgramOutputSinkConfig::disabledColor,
	        Eq(config.disabledColor)
	    ),
	    Field(
	        "forceColor",
	        &ProgramOutputSinkConfig::forceColor,
	        Eq(config.forceColor)
	    )
	);
}

TEST(ProgramOutputSinkConfig, Config) {
	struct TestData {
		std::string                     Name;
		Option<ProgramOutputSinkConfig> Opt;
		ProgramOutputSinkConfig         Expected;
	};

	std::vector<TestData> testdata = {
	    {
	        "WithStdoutOutput",
	        WithStdoutOutput(),
	        buildProgramOutputSinkConfig(true, false, false),
	    },
	    {
	        "WithDisabledColor",
	        WithDisabledColor(),
	        buildProgramOutputSinkConfig(false, true, false),
	    },
	    {
	        "WithForceColor",
	        WithForceColor(),
	        buildProgramOutputSinkConfig(false, false, true),
	    },
	};

	for (const auto &data : testdata) {
		SCOPED_TRACE(data.Name);
		ProgramOutputSinkConfig config;
		data.Opt(config);
		EXPECT_THAT(config, ProgramOutputSinkConfigAreEqual(data.Expected));
	}
}

TEST(Config, Default) {
	Config config{};
	EXPECT_THAT(config.sinks, ::testing::IsEmpty());
}

template <typename... Sinks> Config buildConfig(Sinks &&...sinks) {
	Config config;
	(config.sinks.emplace_back(std::forward<Sinks>(sinks)), ...);
	return config;
}

auto FileSinkConfigAreEqual(const FileSinkConfig &config) {
	using namespace ::testing;
	return Field("filepath", &FileSinkConfig::filepath, Eq(config.filepath));
};

template <typename T> inline constexpr bool always_false_v = false;

auto SinkConfigAreEqual(const SinkConfig &config) {
	using namespace ::testing;

	return std::visit(
	    [](auto &&c) -> ::testing::Matcher<SinkConfig> {
		    using T = std::decay_t<decltype(c)>;
		    if constexpr (std::is_same_v<T, ProgramOutputSinkConfig>) {
			    return VariantWith<ProgramOutputSinkConfig>(
			        ProgramOutputSinkConfigAreEqual(c)
			    );
		    } else if constexpr (std::is_same_v<T, FileSinkConfig>) {
			    return VariantWith<FileSinkConfig>(FileSinkConfigAreEqual(c));
		    } else {
			    static_assert(always_false_v<T>, "non-exhaustive visitor");
		    }
	    },
	    config
	);
}

MATCHER(SinkConfigTupleAreEqual, "") {
	using namespace ::testing;
	return ExplainMatchResult(
	    SinkConfigAreEqual(std::get<1>(arg)),
	    std::get<0>(arg),
	    result_listener
	);
}

auto ConfigAreEqual(const Config &config) {
	using namespace ::testing;
	return Field(
	    "sinks",
	    &Config::sinks,
	    Pointwise(SinkConfigTupleAreEqual(), config.sinks)
	);
}

FileSinkConfig buildFileSinkConfig(
    const std::string &name, bool withLocking = false, bool async = false
) {
	FileSinkConfig config;
	config.filepath    = name;
	config.withLocking = withLocking;
	config.async       = async;
	return config;
}

template <typename T, typename... Options>
Option<T> ConcatOptions(Options &&...options) {
	return [options...](T &config) { (options(config), ...); };
}

TEST(Config, Options) {
	struct TestData {
		std::string    Name;
		Option<Config> Opt;
		Config         Expected;
	};

	std::vector<TestData> testdata = {
	    {
	        "WithProgramOutput",
	        WithProgramOutput(),
	        buildConfig(ProgramOutputSinkConfig{}),
	    },
	    {
	        "WithFileOutput",
	        WithFileOutput("foo.log"),
	        buildConfig(buildFileSinkConfig("foo.log")),
	    },
	    {
	        "Complex",
	        ConcatOptions<Config>(
	            WithProgramOutput(WithStdoutOutput(), WithForceColor()),
	            WithFileOutput("foo.log", WithAsync())
	        ),
	        buildConfig(
	            buildProgramOutputSinkConfig(true, false, true),
	            buildFileSinkConfig("foo.log", false, true)
	        ),
	    },
	};

	for (const auto &data : testdata) {
		SCOPED_TRACE(data.Name);
		Config config;
		data.Opt(config);
		EXPECT_THAT(config, ConfigAreEqual(data.Expected));
	}
}
}; // namespace slog
