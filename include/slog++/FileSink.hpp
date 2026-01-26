#pragma once

#include "Config.hpp"
#include "SinkDetails.hpp"
#include <cstdio>

namespace slog {

template <details::ConcurencyMode CM>
class FileSink : public details::Sink<FileSink<CM>, CM> {
public:
	using Base = details::Sink<FileSink, CM>;

	inline FileSink(const FileSinkConfig &config)
	    : Base(config.levels, config.Formatter()) {
		auto file = std::fopen(config.filepath.c_str(), "a");
		if (file == nullptr) {
			throw std::system_error(errno, std::generic_category());
		}
		d_file = FilePtr(file, [](FILE *f) { std::fclose(f); });
	}

	inline FileSink(const ProgramOutputSinkConfig &config)
	    : Base(config.levels, config.Formatter()) {
		FILE *outputStream = nullptr;
#ifdef _WIN32
		// On Windows, stdout/stderr are macros, so we use the underlying
		// function
		outputStream =
		    config.toStdout ? __acrt_iob_func(1) : __acrt_iob_func(2);
#else
		outputStream = config.toStdout ? stdout : stderr;
#endif
		d_file = FilePtr(outputStream, [](FILE *f) {});
	}

	void Log(const Buffer &buffer) {
		std::fwrite(buffer.data(), sizeof(char), buffer.size(), d_file.get());
		std::fputc('\n', d_file.get());
	}

private:
	using FileCloser = std::function<void(std::FILE *)>;
	using FilePtr    = std::unique_ptr<std::FILE, FileCloser>;

	FilePtr d_file;
};

} // namespace slog
