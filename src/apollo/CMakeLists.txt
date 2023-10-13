include_directories(${PROJECT_SOURCE_DIR}/src)

set(SRC_FILES #
    main.cpp #
    ui/Display.cpp #
    ui/VertexBufferObject.cpp #
    Grabber.cpp #
    StubGrabber.cpp #
)

set(HDR_FILES #
    ui/Display.hpp #
    ui/VertexBufferObject.hpp #
    Grabber.hpp #
    StubGrabber.hpp #
)

set(TEST_SRC_FILES utils/DeferTest.cpp)
set(TEST_HDR_FILES utils/DeferTest.hpp)

add_executable(apollo ${SRC_FILES} ${HDR_FILES})

target_link_libraries(apollo GLEW::glew glfw Eigen3::Eigen)

set_target_properties(apollo PROPERTIES VERSION ${PROJECT_VERSION})

add_executable(apollo-tests ${TEST_SRC_FILES} ${TEST_HDR_FILES})

target_link_libraries(apollo-tests GTest::gtest_main)
gtest_discover_tests(apollo-tests)
add_dependencies(check apollo-tests)
