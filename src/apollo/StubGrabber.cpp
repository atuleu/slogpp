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

void StubGrabber::AnnounceBuffer(Buffer &&buffer) {}

Buffer::Ptr StubGrabber::Grab(int timeout) {
	return Buffer::Ptr(NULL, [](Buffer *) {});
}

} // namespace apollo
} // namespace fort
