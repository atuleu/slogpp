#include "TeeSink.hpp"
#include "gmock/gmock.h"
#include <gmock/gmock.h>

#include "Level.hpp"
#include "Logger.hpp"
#include "MatcherTest.hpp"
#include "MockSink.hpp"

using ::testing::StrictMock;

namespace slog {
class TeeSinkTest : public ::testing::Test {

protected:
	std::shared_ptr<StrictMock<MockSink>> sink1, sink2;

	void SetUp() override;
	void TearDown() override;
};

using ::testing::_;
using ::testing::AtLeast;
using ::testing::Ge;
using ::testing::Return;

void TeeSinkTest::SetUp() {
	sink1 = std::make_shared<StrictMock<MockSink>>();
	sink2 = std::make_shared<StrictMock<MockSink>>();
	EXPECT_CALL(*sink1, Enabled(Ge(Level::Trace)))
	    .Times(AtLeast(size_t(Level::Info) + 1))
	    .WillRepeatedly([](Level l) -> bool { return l >= Level::Info; });
	EXPECT_CALL(*sink2, Enabled(Ge(Level::Trace)))
	    .Times(AtLeast(size_t(Level::Warn) + 1))
	    .WillRepeatedly([](Level l) -> bool { return l >= Level::Warn; });
	;
}

void TeeSinkTest::TearDown() {
	sink1.reset();
	sink2.reset();
}

using ::testing::InSequence;

TEST_F(TeeSinkTest, LogsToAllSink) {
	EXPECT_CALL(*sink1, AllocateOnStack()).WillOnce(Return(true));
	EXPECT_CALL(*sink2, AllocateOnStack()).WillOnce(Return(true));

	auto multi = TeeSink(sink1, sink2);
	EXPECT_TRUE(multi->AllocateOnStack());

	auto logger = Logger<0>(multi);
	{
		InSequence seq;
		EXPECT_CALL(
		    *sink1,
		    Log(AllOf(
		        HasLevel<const Record *>(Level::Warn),
		        HasMessage<const Record *>("a warning"),
		        HasAttributes<const Record *>()
		    ))
		);

		EXPECT_CALL(
		    *sink2,
		    Log(AllOf(
		        HasLevel<const Record *>(Level::Warn),
		        HasMessage<const Record *>("a warning"),
		        HasAttributes<const Record *>()
		    ))
		);
		EXPECT_CALL(
		    *sink1,
		    Log(AllOf(
		        HasLevel<const Record *>(Level::Info),
		        HasMessage<const Record *>("an info"),
		        HasAttributes<const Record *>()
		    ))
		);
	}

	logger.Warn("a warning");
	logger.Info("an info");
}

TEST_F(TeeSinkTest, SharedAllocation) {
	EXPECT_CALL(*sink1, AllocateOnStack()).WillOnce(Return(true));
	EXPECT_CALL(*sink2, AllocateOnStack()).WillOnce(Return(false));

	auto multi = TeeSink(sink1, sink2);
	EXPECT_FALSE(multi->AllocateOnStack());

	using SharedPtr = std::shared_ptr<const Record>;

	auto logger = Logger<0>(multi);
	{
		InSequence seq;
		EXPECT_CALL(
		    *sink1,
		    Log(AllOf(
		        HasLevel<SharedPtr>(Level::Warn),
		        HasMessage<SharedPtr>("a warning"),
		        HasAttributes<SharedPtr>()
		    ))
		);

		EXPECT_CALL(
		    *sink2,
		    Log(AllOf(
		        HasLevel<SharedPtr>(Level::Warn),
		        HasMessage<SharedPtr>("a warning"),
		        HasAttributes<SharedPtr>()
		    ))
		);
	}

	logger.Warn("a warning");
}

TEST_F(TeeSinkTest, PropagateFromAndSet) {
	EXPECT_CALL(*sink1, AllocateOnStack()).WillOnce(Return(true));
	EXPECT_CALL(*sink2, AllocateOnStack()).WillOnce(Return(true));

	EXPECT_CALL(*sink1, Set(_, _)).Times(1);
	EXPECT_CALL(*sink2, Set(_, _)).Times(1);

	EXPECT_CALL(*sink1, From(_)).Times(1);
	EXPECT_CALL(*sink2, From(_)).Times(1);

	auto multi = TeeSink(sink1, sink2);

	multi->From(Level::Info);
	multi->Set(Level::Debug, true);
}

} // namespace slog
