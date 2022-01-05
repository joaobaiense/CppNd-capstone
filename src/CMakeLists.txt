set(BINARY stockSim)

set(SOURCES
    main.cpp
)

set(CMAKE_EXE_LINKER_FLAGS "-pthread")

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})
add_executable(${BINARY} ${SOURCES})

target_link_libraries(${BINARY} PUBLIC
    types
    utils
)
