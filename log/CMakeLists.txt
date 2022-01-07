set(BINARY log)

set(SOURCES
    Log.cpp
)

add_library(${BINARY} STATIC ${SOURCES})

target_include_directories(${BINARY} PUBLIC
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>"
)

target_link_libraries(${BINARY} PUBLIC
    spdlog
)