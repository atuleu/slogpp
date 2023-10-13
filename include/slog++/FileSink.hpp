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
		d_file = FilePtr(config.stdout ? stdout : stderr, [](FILE *f) {});
	}

	void Log(utils::ObjectPool<Buffer>::Ptr &&buffer) {
		std::fwrite(buffer->data(), sizeof(char), buffer->size(), d_file.get());
		std::fputc('\n', d_file.get());
	}

private:
	using FileCloser = std::function<void(std::FILE *)>;
	using FilePtr    = std::unique_ptr<std::FILE, FileCloser>;

	FilePtr d_file;
};

} // namespace slog
