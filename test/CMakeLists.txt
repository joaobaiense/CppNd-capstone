cmake_minimum_required(VERSION 3.12)

set(THIS stockSimTests)

set(CMAKE_EXE_LINKER_FLAGS "-pthread")

set(SOURCES 
    main.cpp
    portfolio/PortfolioTest.cpp
)

ADD_EXECUTABLE(${THIS} ${SOURCES})

target_link_libraries(${THIS} PUBLIC
    gtest_main
    log
    util
    types
)

add_test(
    NAME ${THIS}
    COMMAND ${THIS}
)

# add tests 
add_subdirectory(portfolio)