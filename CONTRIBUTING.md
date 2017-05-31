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

