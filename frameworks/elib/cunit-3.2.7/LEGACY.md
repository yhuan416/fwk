# Historic Documentation

Here you will find documentation on the non-cmake build system for CUnit.  It
may not work at all but is here in case anyone needs it.

## Building the CUnit Library and Examples (Old Style)

_CUnit is moving to CMake, these instructions are historic only and will be removed soon_

### All Platforms:

  As of Version 2.0, a set of Jamfiles is provided for cross-platform
  building of the library, examples, and tests.  The jam build system was
  implemented using ftjam 2.3.2 (http://www.freetype.org/jam/index.html).
  It may work under the original Perforce jam implementation, but has not
  been tested.  It has been tested under gcc on Linux, and Borland C++ 5.5, 
  VC7, MinGW 3.2.3, and Open Watcom 1.3 on Windows.  Due to the nature of 
  jam, the build system should be readily extensible to other platforms.

  The jam file set supports both standard and custom symbolic build
  targets (install, uninstall, clean, libcunit, examples, test).  A
  customized Jambase file is provided which sorts out some of the
  rough edges for MinGW and Watcom on Windows.  It may not be necessary
  when using gcc, Borland, or VC, but it won't hurt either.
  
  The files generated during the build are placed in a subdirectory
  given by `<CONFIG>/<PLATFORM>`, where:
  ```
    <CONFIG> = Debug or Release
    <PLATFORM> = bcc, mingw, msvc, watcom, linux
  ```
  This allows easy switching between compilers without overlap of the output 
  files.  The `<CONFIG>` is determined by whether NODEBUG is defined in your
  Jamrules file.  `<PLATFORM>` is set automatically in Jamrules.

  To build using jam:

    1. Set the working directory to the top of the source tree
    
    2. Generate Jamrules
       a. On Linux, run autoconf & configure
       b. On Windows, copy Jamrules.in to Jamrules

    3. Edit the top section of Jamrules to match your preferences

    4. `jam -f Jambase install`

### Linux:

  In addition to jam, the standard GNU build system is still supported.
  The usual sequence of steps should succeed in building and installing CUnit:
    1. `aclocal`  (if necessary)
    2. `autoconf` (if necessary)
    3. `automake` (if necessary)
    4. `chmod u+x configure` (if necessary)
    5. `./configure --prefix <Your choice of directory for installation>`
    6. `make`
    7. `make install`

  What's installed:
    1. libcunit.a (Library file)
    2. CUnit Header files
    3. DTD and XSL files supporting xml output files in share directory
    4. Man Pages in relevant man directories under the installation path.
    5. HTML users guide in the doc subdirectory of the installation path.
    6. Example & test programs in the share subdirectory of the install path.

### Windows:

  Jam is the preferred build system for Windows.  A set of old VC6 project
  files is included which have been partially updated but not tested.  If
  they don't work and you get them working, we'd be happy to include them
  in the CUnit distribution.  A set of Visual Studio 2003/VC7 solution and
  project files has also been provided in the VC7 subdirectory.  Similarly,
  a set of Visual Studio 2005/VC8 solution and project files is located in
  the VC8 subdirectory.  The latter should work with all versions of VC 2005,
  including the Express edition.
