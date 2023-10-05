#pragma once

#include <memory>

namespace fort {
namespace apollo {

struct Buffer {
	typedef std::unique_ptr<Buffer, void (*)(Buffer *)> Ptr;

	void  *Data;
	size_t Size;
};

class Grabber {

public:
	typedef std::unique_ptr<Grabber> Ptr;

	virtual ~Grabber();

	virtual void AnnounceBuffer(Buffer &&buffer);

	virtual Buffer::Ptr Grab(int timeout = 0) const;

	virtual void Start() = 0;

	virtual void Stop() = 0;
};

} // namespace apollo
} // namespace fort
