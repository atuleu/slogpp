#pragma once

#include <memory>

namespace fort {
namespace apollo {

class Buffer {
public:
	typedef std::unique_ptr<Buffer> Ptr;

	virtual ~Buffer();

protected:
	const void *data;
};

class Grabber {

public:
	typedef std::unique_ptr<Grabber> Ptr;

	virtual ~Grabber();

	virtual void AnnounceBuffer(void *buffer, size_t size);

	virtual Buffer::Ptr Grab(int timeout = 0) const;
};

} // namespace apollo
} // namespace fort
