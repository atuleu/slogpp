set(HDR_FILES Logger.hpp)
set(SRC_FILES Logger.cpp)

set(TEST_HDR_FILES "")
foreach(f ${HDR_FILES})
	list(APPEND TEST_HDR_FILES tests/${f})
endforeach(f ${HDR_FILES})

set(TEST_SRC_FILES "")
foreach(f ${SRC_FILES})
	list(APPEND TEST_SRC_FILES tests/${f})
endforeach(f ${SRC_FILES})

set(TEST_HDR_FILES
	${TEST_HDR_FILES}
	PARENT_SCOPE
)
set(TEST_SRC_FILES
	${TEST_SRC_FILES}
	PARENT_SCOPE
)
