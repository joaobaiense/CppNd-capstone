
#include <gtest/gtest.h>
#include <Log/Log.hpp>

int main(int argc, char **argv) {

    Log::init("TEST");

    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
    
}