#pragma once

#include "Grabber.hpp"
#include "Types.hpp"
#include <deque>
#include <initializer_list>
#include <unordered_map>
#include <vector>

namespace fort {
namespace apollo {
class StubGrabber : public Grabber {
	StubGrabber(const Size &size, float FPS,
	            std::initializer_list<std::string> files);

	void AnnounceBuffer(void *buffer, size_t size) override;

	Buffer::Ptr Grab(int timeout = 0);

private:
	Size  d_size;
	float d_fps;

	std::vector<std::string>                d_files;
	int                                     d_index;
	std::unordered_map<std::string, void *> d_lastRead;

	std::deque<void *> d_fifo;
};

} // namespace apollo
} // namespace fort
