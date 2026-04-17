#pragma once

#include <array>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <filesystem>

#if defined(_MSC_VER)
#include <intrin.h>
#pragma intrinsic(_byteswap_uint64)
#endif

namespace slog {
namespace details {

// This is a small string optimized String class that uses fewer stack
// memory, but has SSO. Inspired by meta's fbstring class.
class String {

	// an helper to ensure that size is BE, so its LSB is the struct LSB when it
	// is the last field.
	static inline constexpr size_t sizeMayBitswap(size_t v) {
		if constexpr (std::endian::native == std::endian::big) {
			return v;
		}
#if defined(__clang__) || defined(__GNUC__)
		return __builtin_bswap64(v);
#elif defined(_MSC_VER)
		return _byteswap_uint64(v);
#else
		portable fallback return (v << 56) |
		    ((v & 0x000000000000FF00ull) << 40) |
		    ((v & 0x0000000000FF0000ull) << 24) |
		    ((v & 0x00000000FF000000ull) << 8) |
		    ((v & 0x000000FF00000000ull) >> 8) |
		    ((v & 0x0000FF0000000000ull) >> 24) |
		    ((v & 0x00FF000000000000ull) >> 40) | (v >> 56);
#endif
	}

public:
	static_assert(
	    sizeof(size_t) == 8 && sizeof(const char *) == 8,
	    "this implementation assumes that we have 64bit pointer and size"
	);

	constexpr String()
	    : d{.small = SmallRep{.available = 15 << 1}} {
		d.small.data[0] = '\0';
	}

	String(const std::filesystem::path &s) {
		initFromString(s.string());
	}

	constexpr String(const std::string &s) {
		initFromString(s);
	}

	constexpr String(const char *c_str) {
		size_t s = 0;
		while (c_str[s] != '\0') {
			++s;
		}

		if (s > 15) {
			d.large.data = new char[s + 1];
			memcpy(d.large.data, c_str, s + 1);
			// size is Big-Endian encoded, so only the last bit should be
			// always set, and it correspond to the LSB of the struct.
			d.large.size_BE = sizeMayBitswap(s | 1);
			return;
		}
		memcpy(d.small.data, c_str, s + 1);
		d.small.available = (15 - s) << 1;
	}

	constexpr ~String() {
		if (isSmall()) {
			return;
		}
		delete[] d.large.data;
	}

	String(const String &other) {
		if (other.isSmall() == true) {
			d.small = other.d.small;
			// SSO: no heap memory, we are done.
			return;
		}
		d.large.data    = new char[other.size() + 1];
		d.large.size_BE = other.d.large.size_BE;
		memcpy(d.large.data, other.d.large.data, other.size() + 1);
	}

	String &operator=(const String &other) {
		if (this == &other) {
			return *this;
		}

		if (other.isSmall() == true) {
			if (isSmall() == false) {
				delete[] d.large.data;
			}
			// SSO: no heap memory, we are done.
			d = other.d;
			return *this;
		}
		// must copy it.
		const auto allocatedSize = other.size() + 1;

		auto newBytes = new char[allocatedSize];

		if (isSmall() == false) {
			delete[] d.large.data;
		}

		d.large.data    = newBytes;
		d.large.size_BE = other.d.large.size_BE;
		memcpy(d.large.data, other.d.large.data, allocatedSize);
		return *this;
	}

	String(String &&other)
	    : d{other.d} {
		// we efficiently empty the other string, keeping for ourself its
		// potential heap memory
		other.reset();
	}

	String &operator=(String &&other) {
		if (this == &other) {
			return *this;
		}

		if (isSmall() == false && d.large.data != nullptr) {
			delete[] d.large.data;
		}

		d = other.d;
		// we efficiently empty the other string, keeping for ourself its
		// potential heap memory
		other.reset();
		return *this;
	}

	size_t size() const noexcept {
		if (isSmall() == true) {
			return 15 - (d.small.available >> 1);
		}
		// size is Big-Endian encoded, so only the last bit should be checked,
		// and it correspond to the LSB of the struct.
		auto s = sizeMayBitswap(d.large.size_BE);
		if (d.large.data[s - 1] == 0) {
			return s - 1;
		}
		return s;
	}

	inline const char *c_str() const noexcept {
		return begin();
	}

	std::string_view string_view() const noexcept {
		return std::string_view(begin(), end());
	}

	bool operator==(const String &other) const noexcept {
		return string_view() == other.string_view();
	}

	inline const char *begin() const noexcept {
		if (isSmall()) {
			return d.small.data;
		}
		return d.large.data;
	}

	inline const char *end() const noexcept {
		if (isSmall()) {
			return d.small.data + size();
		}
		return d.large.data + size();
	}

private:
	inline constexpr void initFromString(const std::string &s) {
		if (s.size() > 15) {
			d.large.data = new char[s.size() + 1];
			memcpy(d.large.data, s.data(), s.size());
			d.large.data[s.size()] = 0;
			// size is Big-Endian encoded, so only the last bit should be
			// always set, and it correspond to the LSB of the struct.
			d.large.size_BE        = sizeMayBitswap(s.size() | 1);
			return;
		}

		memcpy(d.small.data, s.data(), s.size());
		d.small.data[s.size()] = '\0';
		d.small.available      = (15 - s.size()) << 1;
	}

	inline void reset() noexcept {
		d.small         = SmallRep{.available = 15 << 1};
		d.small.data[0] = '\0';
	}

	inline constexpr bool isSmall() const noexcept {

		// check if the LSB of the struct is set. If yes, we are a long string,
		// otherwise a small one. Due to lifetime object of union, in order not
		// to have UB, we need to manually read memory.
		const auto asBytes =
		    std::bit_cast<std::array<uint8_t, sizeof(StackData)>>(d);
		return (asBytes[sizeof(StackData) - 1] & 0x01u) == 0;
	}

	struct SmallRep {
		// fbstring optimization. we keep the available character up to the max.
		// therefore at 15 (the maximal size), the null character will be 0,
		// occupying the last byte of the struct.
		char    data[15];
		// first 7 bits are the remaining character to reach 15 size, the last
		// bit is a flag that indicates a normal string if set.
		uint8_t available;
	};

	struct LargeRep {
		char  *data;
		// size should: 1) be Big-Endian, 2) always be odd. Then its LSB is the
		// struct's LSB.
		size_t size_BE;
		// no capacity. we will never grow and stay immuable.
	};

	// we are either a SSO or a normal String.
	union StackData {
		SmallRep small;
		LargeRep large;
	};

	StackData d;
};

static_assert(sizeof(String) == 16, "this class should be small");

} // namespace details
} // namespace slog
