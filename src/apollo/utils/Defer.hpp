#pragma once

#define fo_DEFER_UNIQUE_NAME_INNER(a, b) a##b
#define fo_DEFER_UNIQUE_NAME(base, line) fo_DEFER_UNIQUE_NAME_INNER(base, line)
#define fo_DEFER_NAME fo_DEFER_UNIQUE_NAME(zz_defer, __LINE__)
#define defer auto fo_DEFER_NAME = fort::apollo::_Defer_void{} *[&]()

namespace fort {
namespace apollo {
template <typename Lambda> struct _Deferrer {
	Lambda lambda;

	~_Deferrer() {
		lambda();
	};
};

struct _Defer_void {};

template <typename Lambda>
_Deferrer<Lambda> operator*(_Defer_void, Lambda &&lambda) {
	return {lambda};
}

} // namespace apollo
} // namespace fort
