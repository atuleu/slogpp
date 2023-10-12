#include <gtest/gtest.h>

#include <slog++/Formatters.hpp>

#include "ObjectPool.hpp"

namespace slog {
namespace utils {

TEST(ObjectPool, Buffer) {
	auto pool = std::make_unique<ObjectPool<Buffer>>();

	EXPECT_NO_THROW({
		auto m1 = pool->Get();
		EXPECT_EQ(pool->Available(), 0);
		EXPECT_EQ(pool->Capacity(), 1);
		auto m2 = pool->Get();
		EXPECT_EQ(pool->Available(), 0);
		EXPECT_EQ(pool->Capacity(), 2);
	});

	EXPECT_EQ(pool->Available(), 2);
	EXPECT_EQ(pool->Capacity(), 2);

	pool.reset();
}

template <typename T> class AllocationCounter {
public:
	inline static size_t constructions = 0;
	inline static size_t destructions  = 0;

protected:
	AllocationCounter() noexcept {
		++constructions;
	}

	virtual ~AllocationCounter() {
		++destructions;
	}
};

TEST(ObjectPool, Allocations) {

	class Object : public AllocationCounter<Object> {
	public:
		Object() noexcept = default;
	};

	auto pool = std::make_unique<ObjectPool<Object>>();

	EXPECT_EQ(AllocationCounter<Object>::constructions, 0);
	EXPECT_EQ(AllocationCounter<Object>::destructions, 0);

	pool->Reserve(1);
	EXPECT_EQ(AllocationCounter<Object>::constructions, 1);
	EXPECT_EQ(AllocationCounter<Object>::destructions, 0);

	EXPECT_NO_THROW({
		auto m1 = pool->Get();
		EXPECT_EQ(AllocationCounter<Object>::constructions, 1);
		EXPECT_EQ(AllocationCounter<Object>::destructions, 0);
		auto m2 = pool->Get();
		EXPECT_EQ(AllocationCounter<Object>::constructions, 2);
		EXPECT_EQ(AllocationCounter<Object>::destructions, 0);
	});
	EXPECT_EQ(AllocationCounter<Object>::constructions, 2);
	EXPECT_EQ(AllocationCounter<Object>::destructions, 0);
	EXPECT_EQ(pool->Capacity(), 2);
	EXPECT_EQ(pool->Available(), 2);

	pool.reset();

	EXPECT_EQ(AllocationCounter<Object>::constructions, 2);
	EXPECT_EQ(AllocationCounter<Object>::destructions, 2);
}

} // namespace utils
} // namespace slog
