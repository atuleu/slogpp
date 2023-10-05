#pragma once

#include <cstddef>

namespace fort {
namespace apollo {

struct Size {
	size_t Width, Height;

	size_t ByteSize() const {
		return Width * Height;
	};
};

} // namespace apollo
} // namespace fort
