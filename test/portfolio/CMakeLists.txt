cmake_minimum_required(VERSION 3.12)

set(THIS portfolioTest)

set(SOURCES
    PortfolioTest.cpp
)

set(CMAKE_EXE_LINKER_FLAGS "-pthread")

ADD_LIBRARY(${THIS}_lib STATIC ${SOURCES})

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})
TARGET_LINK_LIBRARIES(${THIS}_lib PUBLIC 
    gtest
    log
    types
)

ADD_EXECUTABLE(${THIS} ${SOURCES} PortfolioTestMain.cpp)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})
TARGET_LINK_LIBRARIES(${THIS} PUBLIC 
    gtest
    log
    types
)

add_test(
    NAME ${THIS}
    COMMAND ${THIS}
)
