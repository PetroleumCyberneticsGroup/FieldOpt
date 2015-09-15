#include <gtest/gtest.h>
#include "eclgridreader.h"

using namespace ERTWrapper;

namespace {

class ECLGridReaderTest : public ::testing::Test {
protected:
    ECLGridReaderTest() {
        // Set-up work for each test
    }

    virtual ~ECLGridReaderTest() {
        // Clean-up work that does not throw exceptions.
    }

    virtual void SetUp() {
        // Called immediately after the constructor (right before each test)
    }

    virtual void TearDown() {
        // Called immedeately after each test (right before the destructor)
    }

    // Objects declared here can be used by all tests in this test case.
};

TEST_F(ECLGridReaderTest, TestSomething) {
    EXPECT_EQ(0,0);
}

}
