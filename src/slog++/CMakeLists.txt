set(HDR_FILES
    Logger.hpp #
    Attribute.hpp #
    Level.hpp #
    Record.hpp #
    Formatters.hpp #
    Types.hpp #
    Config.hpp #
    utils/ContainerReference.hpp #
    utils/ObjectPool.hpp #
    utils/ThreadPool.hpp #
    slog++.hpp #
    Sink.hpp #
)

set(SRC_FILES
    LoggerImpl.hpp #
    AttributeImpl.hpp #
    RecordImpl.hpp #
    FormattersImpl.hpp #
    ConfigImpl.hpp #
    utils/ContainerReferenceImpl.hpp #
    utils/ObjectPoolImpl.hpp #
    utils/ThreadPoolImpl.hpp #
    slog++Impl.hpp #
    SinkDetails.hpp #
    FileSink.hpp #
)

set(TEST_SRC_FILES
    LoggerTest.cpp #
    AttributeTest.cpp #
    FormattersTest.cpp #
    ConfigTest.cpp #
    utils/ObjectPoolTest.cpp #
    slog++Test.cpp #
)

set(TEST_HDR_FILES
    LoggerTest.hpp #
    AttributeTest.hpp #
    MockSink.hpp #
)

include_directories(${PROJECT_SOURCE_DIR}/src)

set(SLOGPP_CPP_CONTENT "")
foreach(src ${SRC_FILES})
  set(SLOGPP_CPP_CONTENT "${SLOGPP_CPP_CONTENT}#include \"${src}\"\n")
endforeach(src ${SRC_FILES})

configure_file(slog++.cpp.in slog.cpp @ONLY)

add_executable(slog++-tests ${HDR_FILES} ${SRC_FILES} ${TEST_HDR_FILES}
                            ${TEST_SRC_FILES})

target_link_libraries(slog++-tests GTest::gtest_main GTest::gmock_main)

gtest_discover_tests(slog++-tests)
add_dependencies(check slog++-tests)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
add_library(slog++ STATIC ${HDR_FILES} ${SRC_FILES} slog.cpp)
