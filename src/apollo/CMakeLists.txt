set(SRC_FILES main.cpp)

set(HDR_FILES)

add_executable(apollo ${SRC_FILES} ${HDR_FILES})

set_target_properties(apollo PROPERTIES VERSION ${PROJECT_VERSION})
