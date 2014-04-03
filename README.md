denali - a tool for visualizing scalar trees

Installation
================================================================================

Linux
--------------------------------------------------------------------------------

### From source
The build process is managed by CMake. To build from source and install to the 
default location under `/usr/local`, invoke from the project's root directory:

    mkdir build
    cd build
    cmake ..
    make
    sudo make install

To install to a location other than the default, specify a prefix by using
CMake's `CMAKE_INSTALL_PREFIX`. For example, to install to user `eldridge`'s
home directory, run

    mkdir build
    cd build
    cmake .. -DCMAKE_INSTALL_PREFIX=/home/eldridge
    make
    make install
