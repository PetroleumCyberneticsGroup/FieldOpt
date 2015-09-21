# Unit Tests

Test are as of sept. 2015 created using the Google Test framework.

Directions for setting up the library on Ubuntu and creating simple tests
may be found at http://www.eriksmistad.no/getting-started-with-google-test-on-ubuntu/

## Writing And Running Tests

Main guidelines:

* Each new class should have a set of tests.
* Tests for each class should be in a separate file named `test_classname.cpp` in a directory reflecting the class' placement in the project tree. For example, the tests for the `Grid` class located in `Library/model/grid/grid.*` should have its tests located in /GTest/Library/model/grid/test_grid.cpp`.
* Tests should be written by using fixtures. See the boilerplate code in the end of this readme for an example, or look at the `test_grid.cpp` file.

Each of the separate should be included in the main test file `gtest_main.cpp`.

Tests are run by executing the `run_tests.sh` script.

## Test Assets

### ECLIPSE HORZWELL Model

The HORZWELL model found in `examples/ECLIPSE/HORZWELL` is a reservoir model with a 20x9x9 grid, spanning
* x = [0:2000]
* y = [0:990]
* z = [7000:7275]

## GTest Boilerplate

This boilerplate code is taken from https://code.google.com/p/googletest/wiki/Primer
At this site you can also find some additional documentation on writing tests using the googletest framework.

```
#include "this/package/foo.h"
#include "gtest/gtest.h"

namespace {

// The fixture for testing class Foo.
class FooTest : public ::testing::Test {
 protected:
  // You can remove any or all of the following functions if its body
  // is empty.

  FooTest() {
    // You can do set-up work for each test here.
  }

  virtual ~FooTest() {
    // You can do clean-up work that doesn't throw exceptions here.
  }

  // If the constructor and destructor are not enough for setting up
  // and cleaning up each test, you can define the following methods:

  virtual void SetUp() {
    // Code here will be called immediately after the constructor (right
    // before each test).
  }

  virtual void TearDown() {
    // Code here will be called immediately after each test (right
    // before the destructor).
  }

  // Objects declared here can be used by all tests in the test case for Foo.
};

// Tests that the Foo::Bar() method does Abc.
TEST_F(FooTest, MethodBarDoesAbc) {
  const string input_filepath = "this/package/testdata/myinputfile.dat";
  const string output_filepath = "this/package/testdata/myoutputfile.dat";
  Foo f;
  EXPECT_EQ(0, f.Bar(input_filepath, output_filepath));
}

// Tests that Foo does Xyz.
TEST_F(FooTest, DoesXyz) {
  // Exercises the Xyz feature of Foo.
}

}  // namespace

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
```