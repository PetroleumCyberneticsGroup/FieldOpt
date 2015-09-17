#include <iostream>
#include <gtest/gtest.h>
#include "ERTWrapper/test_eclgridreader.cpp"

using namespace std;

GTEST_API_ int main(int argc, char **argv) {
    printf("Running main() from gtest_main.cc\n");
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
