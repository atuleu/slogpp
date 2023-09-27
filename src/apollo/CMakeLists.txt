set(SRC_FILES main.cpp Display.cpp)

set(HDR_FILES Display.hpp)

add_executable(apollo ${SRC_FILES} ${HDR_FILES})

target_link_libraries(apollo GLEW::glew glfw)

set_target_properties(apollo PROPERTIES VERSION ${PROJECT_VERSION})
