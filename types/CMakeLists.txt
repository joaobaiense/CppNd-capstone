set(BINARY types)

set(SOURCES
    Ochl.cpp
    Portfolio.cpp
)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})

add_library(${BINARY} STATIC ${SOURCES})
target_include_directories(${BINARY} PUBLIC
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>"
)

target_link_libraries(${BINARY} PUBLIC
    utils
)