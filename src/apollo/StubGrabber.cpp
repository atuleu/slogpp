#include <initializer_list>
#include <stdexcept>

#include "StubGrabber.hpp"
#include "Types.hpp"

#include <apollo/utils/Defer.hpp>

namespace fort {
namespace apollo {
StubGrabber::StubGrabber(const Size &size, float FPS,
                         std::initializer_list<std::string> files)
    : d_size(size), d_fps(FPS), d_files(files), d_index(0) {
	for (auto file : d_files) {
		d_lastRead[file] = nullptr;
	}
}

void StubGrabber::AnnounceBuffer(void *buffer, size_t size) {
	if (size < d_size.ByteSize()) {
		throw std::invalid_argument(
		    "Invalid buffer size " + std::to_string(size) +
		    "needed : " + std::to_string(d_size.ByteSize()));
	}

	d_fifo.push_back(buffer);
}

Buffer::Ptr StubGrabber::Grab(int timeout) {

	defer {
		d_index = (d_index + 1) % d_files.size();
	};

	if (d_lastRead[d_files[d_index]] == d_fifo.front()) {
		return std::make_unique<Buffer>(new Buffer{d_fifo.front()});
	}
}

} // namespace apollo
} // namespace fort
