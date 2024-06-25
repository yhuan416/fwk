
# CUnit : A Unit Testing Framework
			               
http://gitlab.com/cunity/cunit - forked from http://cunit.sourceforge.net

CUnit is a Unit testing framework for C.

The basic framework is platform/version independent and should be
portable to all platforms.  CUnit provides various interfaces to
the framework, some of which are platform dependent (e.g. curses on
*nix).  Building other interfaces should be straightforward with
the facilities provided in the framework.

CUnit is built as either a static or shared library which provides 
framework support when linked into user testing code.  The framework 
complies with the conventional structure of test cases bundled into 
suites which are registered with the framework for running.  See the
documentation for more about the structure and use of the framework.

Note - the windows-specific gui interface is not yet written.  It is
still necessary to use either the automated, basic, or console
interfaces to CUnit on Windows at this time.

# New Releases of CUnit - 2018-08 Onwards

This is a fork of CUnit from the SourceForge version. The fork may heal
eventually but as of today I have not been able to contact the original
maintainers of CUnit.

_Cunity Cunit_ will maintain a *classic-cunit* branch that from now onwards
only contains bugfixes and minor changes. This should be suitable as a 
drop-in for anyone formerly using the sourceforge git repo. This will only
get small changes by-request from 2018-08-20 onwards.

The _classic-cunit_ branch will remain buildable with Jam as with the
sourceforge version but will only have light testing.

The _master_ branch will be the latest code to pass the automated Gitlab
CI build and test.  The _master_ branch will focus on CMake as it's sole 
build system and will gradually remove the old Jam/Make configurations as
more of the project gets included in the CMake builds.

On gitlab.com, CUnit is regularly built on the following platforms:

  * Ubuntu AMD64 18.04
  * Ubuntu AMD64 17.10
  * Ubuntu Power9 19.04
  * Solaris AMD64 11 
  * Windows VS2015

## Building CUnit

CUnit now builds using CMake (http://www.cmake.org) as such, it should build
on all cmake platforms including windows and linux without any changes (if 
it does not please log a bug)

Eg, on linux, you would do:-

```
mkdir local-build
cd local-build
cmake ..
cmake --build .
```

The above should result in a `./CUnit/Sources/libcunit.a` file for your platform
and a self-test program at `./CUnit/Sources/cunit_test`

### Using CUnit as a submodule

It is not always possible to build and install a package into the system you are using.
If using CMake, you can include cunit as a submodule and compile it using
an `add_subdirectory()` call.

```
git submodule add ... cunit
```
Then use.
```
add_subdirectory(cunit/CUnit)

...

target_link_libraries(mytest PRIVATE cunit)
```


# Quick Start

You want to write a modern CUnit test compatible with common CI/CD workflows?

Here is the absolute simplest test program you can write.


```
#include "CUnit/CUnitCI.h"

/* Test that one equals one */
static void test_simple_pass1(void) {
    CU_ASSERT_FATAL(1 == 1);
}

CUNIT_CI_RUN("my-suite",
             CUNIT_CI_TEST(test_simple_pass1));
```

When executed, the above program will write a JUnit XML report and
print output similar to:

```
Starting CUnit test:
 /home/inb/git/cunit/cmake-build/Examples/CI/cicd-pass-plain
JUnit XML:
 /home/inb/git/cunit/cmake-build/Examples/CI/cicd-pass-plain-Results.xml

Running Suite : cicd-pass-plain
     Running Test : test_simple_pass1 ..PASSED

Run Summary:    Type  Total    Ran Passed Failed Inactive
              suites      1      1    n/a      0        0
               tests      1      1      1      0        0
             asserts      1      1      1      0      n/a

Elapsed time =    0.000 seconds
```

A CUnit program written in this way will exit with a status value of zero if all the test have passed. This means you can easily include it in part of a CI script to fail a build early.

## Setups and Teardowns

If you want to make use of setup and teardown functions (per suite or per test setup/cleanup) you can use the
SETUP and TEARDOWN macros:-

```
#include "CUnit/CUnitCI.h"

char *buf = NULL;
size_t bufsize = 32;

/* run at the start of the suite */
CU_SUITE_SETUP() {
    buf = (char*) malloc(bufsize);
    CU_ASSERT_FATAL(buf != NULL);
    return CUE_SUCCESS;
}

/* run at the end of the suite */
CU_SUITE_TEARDOWN() {
    if (buf) free(buf);
    return CUE_SUCCESS;
}

/* run at the start of each test */
CU_TEST_SETUP() {
    memset(buf, 1, bufsize);
}

/* run at the end of each test */
CU_TEST_TEARDOWN() {
    memset(buf, 0, bufsize);
}

/* Test that one equals one */
static void test_simple_pass1(void) {
    CU_ASSERT_FATAL(1 == 1);
}

/* Test that two is bigger than one */
static void test_simple_pass2(void) {
    CU_ASSERT_FATAL(2 > 1);
}

CUNIT_CI_RUN("my-suite",
             CUNIT_CI_TEST(test_simple_pass1),
             CUNIT_CI_TEST(test_simple_pass2),
             );
```

# Online Documentation

You should be able to reach the latest doxygen documentation at http://cunity.gitlab.io/cunit 