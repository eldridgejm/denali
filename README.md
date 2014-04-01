denali
======
A tool for visualizing scalar trees

Installation
================================================================================

Linux
--------------------------------------------------------------------------------
The recommended method of installation on Linux is from source. The build
process is managed by CMake. You'll also need VTK and Qt. To build denali,
create a build directory in this folder, for example, `build`. Then, in a shell,
run:

    cd build
    cmake ..
    make

The built binaries will exist under `./build/qtgui` and `./build/ctree`.
