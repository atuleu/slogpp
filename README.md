# slogpp: C++ structured logging

Structured Logging in C++ inspired by Golang's slog / zap.

## About The Project

slogpp is designed for efficient logging in a structured format with multiple outputs, making it ideal for distributed and micro-architecture systems. Each component can produce its own logs, which can be merged online using telemetry tools. While slogpp doesn't implement these tools, it is easy to integrate and interface with them.

### Feature Set

* **Header-Only Implementation**: slogpp is a header-only library with minimal dependencies, allowing seamless integration with any build system and heavy reliance on template/generic programming to ensure performance and ease of use.

* **Dry and Manageable Logging**: slogpp helps keep logging code Don't Repeat Yourself (DRY) and manageable, ensuring all required fields are populated in complex workflows. For instance, instead of repeating the logging of the `"url"` field, you can create a logger with predefined fields and reuse it throughout your code, making your codebase cleaner and more maintainable.

  ```cpp
  void HandleRequest(const Request &r) {
    auto logger = slog::With(slog::String("request", r.url));
    try {
      // do something that may throw, such as a DB request, and formatting a response, each nested in its own hierarchies of functions. If you do not pass by reference the the produced logger, it will be very hard to log additional field in each of the subcall.
      logger.Info("request successful", slog::Int("status", 200));
    } catch(std::exception &e) {
      logger.Error("request failed", slog::Int("status", 501), slog::Error(e));
    }
  ```
* **Multiple Format Output**: slogpp supports multiple output formats such as JSON and plain text.

## Getting Started
### Prerequisites
The only requirement for using slogpp is a C++17 or later compiler. The library makes extensive use of std::variant, which is available in C++17 and later.

### Basic Usage
Example Usage
To use slogpp in your project, you can follow these steps:

1. Include the necessary header files in your code base.
```cpp
#include <slog++/slog++.hpp>
```
2. Create loggers with predefined fields as needed in your code.
```cpp
auto logger = slog::With(slog::String("component", "http"));
```

3. Use the logger to record log events.
```cpp
logger.Info("request successful", slog::String("request",r.URL), slog::Int("status", 200));
```

Alternatively, you can use the static logging function `slog::Info` `slog::Warn`, `slog::Error` ... to use the default logger which does not have prediefined fields.

### Extended Usage

#### 1. Change the Sink

You can change the default logger sink to another location.
```cpp
slog::SetSink(slog::BuildSink(slog::WithFileOutput("/tmp/log.json",
                              slog::FromLevel(slog::Level::Debug),
                              slog::WithFormat(slog::OutputFormat::JSON));
// The later slog::WithFormat can be omitted as JSON is the default output format when creating a sink
```

The default logger, used by the static function `slog::With` and `slog::Log` defaults to output on stderr in text format (colorized if outputing to a tty).

#### 2. Creating different logger

You can create your own logger instead of deriving the default one:

```cpp
auto stderrSink = slog::BuildSink(
    WithProgramOutput(
        slog::WithFormat(slog::OutputFormat::TEXT),
        slog::FromLevel(slog::Level::Info)
     )));

auto fileSink = slog::Logger<0>(slog::BuildSink(
    slog::WithFileOutput(
        "/tmp/log.json",
        slog::FromLevel(slog::Level::Debug)
    )));

// A logger to stderr
auto loggerConsole = slog::Logger<0>(stderrSink); // note number of default logger fields as template parameter.

// A logger to "/tmp/log.json"
auto loggerFile = slog::Logger<0>(fileSink);

// A logger to both
auto logger = slog::Logger(std::make_shared<slog::MultiSink(stderrSink,fileSink));

// A Logger that outputs to both stderr and a file with different levels:
auto logger = slog::Logger<0>(slog::BuildSink(
    WithProgramOutput(
        slog::WithFormat(slog::OutputFormat::TEXT),
        slog::FromLevel(slog::Level::Info)
     ),
    slog::WithFileOutput(
        "/tmp/log.json",
        slog::FromLevel(slog::Level::Debug)
    )));

loggerConsole.Info("Hello console!"); // only on stderr
loggerFile.Info("Hello file!"); // only on 
logger.Info("Hello World!");
```

## Benchmarks

We have not yet included benchmarks for the project. Performance evaluation is a part of our future plans.

## Roadmap

See the open issues for a full list of proposed features and known issues.

## Contributing

Contributions are welcome and greatly appreciated. Please fill free to fork and submit a PR.

Don't forget to give the project a star! Thanks for your support.

## License
Distributed under the MIT License. See LICENSE for more information.

  

