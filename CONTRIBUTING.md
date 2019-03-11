This is the contribute.md of the FieldOpt project. Great to have you here.
Here are a few ways you can help make this project better!

# Contribute.md

## Team members

The core team members in the FieldOpt project are @einar90 and @Bellout.

## Adding new features

If you want to add new features to FieldOpt, it is greatly appreciated. However,
there are some guidelines we would like you to follow, to make the process 
easier for everyone involved.

### Style Guidelines

We do our best to stick to the
[Google C++ Style](https://google.github.io/styleguide/cppguide.html). This can
be selected as a style in the CLion IDE to provide appropriate auto-indentation.

### Documenting Your Code

When you add new classes or functions, or modify existing ones, remember to add
or update the Doxygen comments in the header files. These comments make up the
primary documentation for FieldOpt, and they can be compiled to HTML using the
Doxygen program for easier reading.

### Testing Your Code

If you have added something significant, you should add unit tests for it. If
you have modified something, update the existing unit tests, and add new ones if
you can.

The unit tests in FieldOpt are written using
[Google Test](https://github.com/google/googletest), and the tests are found in
a `tests` subdirectory in each module directory (e.g.
[here](https://github.com/PetroleumCyberneticsGroup/FieldOpt/tree/develop/FieldOpt/Optimization/tests))

### Pull Requests

We prefer that contributions are submitted in the form of pull requests. Pull
requests should:

* Contain working code. At the very least the program must compile, and,
preferably, all unit tests should pass.
* Be limited to _one_ feature. If a pull request contains a lot of changes in
many places, spotting errors is harder and code review will take longer.
* Contain a good description of what has been done. In the description of your
pull request, write a clear description of what you've done and why you did it.
* Not mix formatting and functional changes. Running auto-indent and adding
license headers in files that you have made changes to anyway is fine.
Large-scale reformatting and reformatting that includes line-breaks should be
kept to a minimum and ideally be in separate pull requests. This is in order to
ease code review.

#### For the Reviewers

Reviewers should:

* Make sure that the program compiles and the unit tests all pass. Code that
breaks tests should never be merged.
* Review the code and make sure that it follows style guidelines and is
sufficiently documented and tested.
* _Not_ commit changes directly to the branch, unless the submitter has asked
for it. Instead, if you want something changed or added, add a comment; or if
you feel you _have_ to, create a new branch and a pull request to the branch in
question.




# FieldOpt Code Style Guidelines

* These guidelines are largely based on the [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html).
* While these guidelines are not followed to the letter everywhere in the current codebase, we should strive
to follow them in any new code written.

## Overview

* Classes and files.
* Members and variables.
* Methods and functions.
* Column width and line-breaks.
* Indentation.
* Comments
* Using `const`.
* Default argument values.
* Tests


# Classes and Files 

Classes should have descriptive (but not too long) names.
They should be written in *camel case*, e.g.

```cpp
class BoundaryConstraint { ... }
```

File names should be written in *snake case* and generally have the same
name as the class whose definition they contain, e.g.

```
boundary_constraint.h
boundary_constraint.cpp
```

* Abbreviations should generally be avoided (e.g. use `PolarWellSpline`, not `PWS`).
* Classes should be implemented in pairs of `x.h, x.cpp` files.
* `x.hpp` files should be self-contained headers with no corresponding `x.cpp` file.


# Members and variables

Members (member variables) are variables that are part of a class.
They should be written in *snake case* with a trailing underscore
and have descriptive names.

```cpp
class BoundaryConstraint {
 private:
  int min_j_;
  QUuid affected_variable_;
  bool is_initialized_;
}
```

Local variables are variables used locally inside methods/functions.
This also includes function arguments.
They should have descriptive names and be written using snake case.

```cpp
void BoundaryConstraint::SnapCaseToConstraints(Case *c) const {
    int local_variable;
    bool is_successful;
}
```

* Classes should **never** have public members. Use getters and setters.
* Boolean variables should generally start with `is_`.

# Methods and functions

## Methods

Methods (i.e. functions inside class) should _always_ have descriptive names and
be written using camel case.

* _Public_ methods: upper case first letter.
* _Private_ methods: lower case first letter.

## Getters and setters

Getters and setters are public methods that retrieves or alters the value of a
member variable. Note that a member often does not need a public setter, only a
getter.

* A setter name should be prefixed with `Set` followed by the name of the member
variable written in camel case.
* A getter should have the name of the member variable written in camel case.
In some cases this may cause confusion, in which case the name should be
be prefixed with `Get`.
* Getters for boolean values should be prefixed with `Is`.

## Arguments

The naming rules for arguments are the same as for local variables (this is also
what they really are).

## Example: Method naming

```cpp
namespace Optimization {
class BoundaryConstraint {
 public:
  void SnapCaseToConstraints(Case *c); // Public method
  
  int MinJ() const; // Getter for min_j_
  void SetMinJ(const int &min_j); // Setter for min_j_
  
  bool IsInitialized(); // Getter for is_initialized_

 private:
  void initialize(); // Private method
 
  int min_j_;
  bool is_initialized_;
}
}
```

**Note:** These practices have not been followed everywhere.
In some places it's because methods have been changed from private to public.
But in most places this is because I've changed my mind too many times on how I
like it.


## Functions

Pure functions/procedures should only be found inside `.hpp` files.
These are utility functions for math, time, string handling, etc.
They should should be written using all lower-case letters and camel case
in order to distinguish them from methods.

```cpp
virtual void get_random_integer(const int &min, const int &max, gen &rng) {
   // implementation
 }
```


# Indentation

Use the guidelines provided by the Google C++ Style Guide for indentation.
These can be applied as a style (used when generating code or auto-indenting)
in CLion: File -> Settings -> Editor -> Code Style -> C/C++.
Set from... -> Predefined Style -> Google.

Briefly:

* Indent with spaces, not tabs.
* Indentation for namespaces: none.
* Indentation in general (anything inside braces): 4 spaces.
* Indentation in class declaration: 1 for `public/private/protected`, two for
the rest (see previous example).
* Continuation indentation: 4, or fit to above level.

```cpp
void DoSomething() {
    if (a == b) {
        a = b;
    }
    DoSomethingElse(member_with_long_name_,
                    another_member_with_long_name_);
    variable_c = member_with_long_name_ + another_member_with_long_name_
        + yet_another_member_with_long_name_;
}
```

# Column width and line-breaks

Which column width is used is largely a matter of taste, but we should be
consistent about it and it should be a compromise between readability and
taste in window size.

The default in CLion is 120 columns (characters) and we should stick to that as
the hard limit (i.e. never write lines longer than that).

* **Vertical space is more valuable than horizontal space**.
* "One operation pr. line" is a good rule to follow. Don't break a line without
good reason. When reading code you shouldn't have to read much more than the
name of the method being called to understand what's happening: you only care
about the arguments when writing/debugging the code and thus should not devote
too much vertical space to it in the committed code.
* In some cases it is reasonable to insert line breaks before the hard limit,
e.g. if a function has many arguments. 
* More line breaks is not always better, as they mean more scrolling to read the
code, which makes it more difficult to get an overview.
* Don't insert or remove line breaks in existing code unless you're changing it
anyway. This just makes for a less readable git diff.
* _Comments:_ When writing comments, in particular doxdoc, we're more worried
about the actual text being readable. In this case, something in the 50-80 range
is reasonable.


## Column width and line breaks: Examples

```cpp

// 120 Columns: ------------------------------------------------------------------------------------------------------|

// Too long:
paths_.SetPath(Paths::SIM_HDF5_FILE, paths_.GetPath(Paths::SIM_WORK_DIR) + "/" + driver_file_name_.split(".").first() + ".vars.h5");

// Acceptable split, by arguments, indented to parenthesis:
paths_.SetPath(Paths::SIM_HDF5_FILE,
               paths_.GetPath(Paths::SIM_WORK_DIR) + "/" + driver_file_name_.split(".").first() + ".vars.h5");

// A Constructor with many arguments, can't possibly fit on one or two lines.
// Insert linebreaks in logical places, but not at every comma.
Cell::Cell(int global_index, IJKCoordinate ijk_index,
           double volume, vector<double> poro_in,
           vector<double> permx_in, vector<double> permy_in, vector<double> permz_in,
           double dx, double dy, double dz,
           Eigen::Vector3d center,
           vector<Eigen::Vector3d> corners,
           int faces_permutation_index,
           bool active_matrix, bool active_fracture, int k_fracture_index)

// A function with many arguments. While long, it fits on one line, and "one operation pr. line" should
// be adhered to when possible.
RigCellGeometryTools::findCellLocalXYZ(hexCorners, iAxisDirection, jAxisDirection, kAxisDirection);
```




# Using `const`

The `const` keyword should be used as often as possible in function signatures.
It signifies immutability of either an argument or a class depending on where
it is placed.

This makes it more clear what a method can do, and ensures that variables
do not change in unexpected/unintended ways.

```cpp
// The method cannot modify the class it is part of.
int getter() const;

// The method cannot modify the referenced variable i, but it can modify the class the method is part of.
int setter(const int &i);

// The method cannot modify either referenced variable or the class it is part of.
int procedureA(const int &a, const int b&) const;

// The method cannot modify the referenced variable b or the class the method is part of,
// but it may modify the referenced variable a.
void procedureB(int &a, const int &b) const;
```

# Default Argument Values

Setting default argument values are sometimes a good idea, but it should not be
overused.
A good example of when to use default values are when a function will be called
with the same argument value in most (but not all) places in the code.
Setting a default argument value can then both help reduce programming errors
and reduce the volume of code needed to call the function.

Another scenario is if you need to add arguments to a function for a new
use-case.
Adding arguments with default values at the end of the signature then means that
you don't have to modify existing calls to that function.

## Use case: Optionally providing RNG seed

The `get_random_generator` function has a default value for the seed.
If called without an argument, i.e. `get_random_generator()`, a `random_device`
will be used to generate a random seed.
However, if called _with_ an argument, e.g. `get_random_generator(5)`, the seed
passed to the function will be used in creating the generator.
```cpp
inline boost::random::mt19937 get_random_generator(int seed = 0) {
   if (seed == 0) {
       boost::random_device dev;
       boost::random::mt19937 gen(dev);
       return gen;
   }
   else {
       boost::random::mt19937 gen(seed);
       return gen;
   }
}
```

## Use case: Adding arguments to Optimizer constructor

In the original use, this constructor is called without the last two arguments
(the case and constraint handlers are created in the constructor).
The hybrid optimzer will call the constructor with the last two arguments,
enabling sharing of handlers between different optimizers.

Using the defaulted arguments means that old calls to optimizer constructors
did not have to change, while at the same time allowing for the new use
in the hybrid optimization implementation.

```cpp
GSS(Settings::Optimizer *settings,
    Case *base_case,
    Model::Properties::VariablePropertyContainer *variables,
    Reservoir::Grid::Grid *grid,
    Logger *logger,
    CaseHandler *case_handler=0,
    Constraints::ConstraintHandler *constraint_handler=0
);
```

# Comments

We distinguish between two kinds of comments: Doxydoc comments (used by Doxygen
to generate the documentation) found in header files, and "normal" inline
comments found in `cpp` files.

## Doxydoc

**ALL** header files must contain doxydoc-comments describing their contents in
some way.

* `hpp` files must have one doxydoc comment pr. function, describing that
that function.
* Normal header files must have one doxydoc comment describing the class
implemented, as well as one comment for each method and non-trivial member in
the class.
    * Exception: trivial getters and setters do not need comments.

### Format of Doxydoc comments

* Doxydoc comments start with `/*!` and end with `*/`.
* Tags may be prefixed with either `\ ` or `@`: `\brief`, `@param`.
* Doxydoc comments may also be written inline. This is especially useful for
member variables. These start with `//!<`, e.g.: `int member_; //!< Member description.`

### Example of Doxydoc

```cpp
/*!
 * @brief Brief description of class goes here.
 * The brief description may span multiple lines.
 *
 * Longer description of the class goes here (notice the empty line after the
 * brief description.
 */
class ClassA {
 public:
  /*!
   * @brief Brief decription of MethodOne.
   * 
   * Detailed description of MethodOne.
   * @param a Brief description of parameter a.
   * @param b Brief description of parameter b.
   * @return Description of what MethodOne returns.
  int MethodOne(int a, int b);

 private:
   int member_; //!< Brief description of member_.
}
```

## Inline Comments
Inline comments in the code should, in general, not be necessary.
The code should be _self-documented_ by the variable- and function names.
If this is not the case, first rethink the naming.

If you feel like it is necessary anyway, keep it short and don't insert
unnecessary whitespace in or around the comment.

# Tests

* _Always_ write new unit tests when implementing new functionality.
* _Always_ run _all_ unit tests after modifying code.
* _Never_ create a pull request with tests that do not pass (this will make me
cranky).
* If your new code makes a test somewhere else in the code, either fix or, if
you can't fix it, note in the pull request which test it breaks.
* The easiest way of creating tests for a new class is to just copy the contents
of an existing test file and modify the names.

## Examples: Tests

```cpp
class TestSuiteName : public testing::Test, public TestResources::TestResourceModel {
protected:
    TestSuiteName() {
        // Constructor for the test suite. Called once. Use it to set member variables available to all tests.
        well_ = settings_model_->wells().first();
    }
    virtual ~TestSuiteName() { /* Called after all tests have run. */ }
    virtual void SetUp() { /* Called before each test. */ }
    virtual void TearDown() { /* Called after each test. */ }

    ::Settings::Model::Well well_;
};

TEST_F(TestSuiteName, TestName) {
    EXPECT_NO_THROW(well_.GetControls()); // Expect no throws from method call.
    EXPECT_STREQ("PROD", well_.name.toLatin1().constData()); // Compare strings.
    EXPECT_EQ(0, 1-1); // Expect the two arguments to be equal.
    EXPECT_TRUE(well_.all_controls_.first()->open()); // Expect the the argument value to be true.
    EXPECT_FLOAT_EQ(100, well_.all_controls_.first()->bhp()); // Compare floats in arguments.

    // Expect arguments to be near each other, with the value provided as
    // the third argument as the allowed deviation.
    EXPECT_NEAR(100, well_.all_controls_.last()->bhp(), 10);
}
```
