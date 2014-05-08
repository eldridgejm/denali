# Installation

- [Linux](#linux)
    - [From source](#from-source)

## Linux

### From source
The build process is managed by CMake. To build from source and install to the 
default location under `/usr/local`, invoke from the project's root directory:

~~~~~~ {.bash}
mkdir build
cd build
cmake ..
make
sudo make install
~~~~~~

To install to a location other than the default, specify a prefix by using
CMake's `CMAKE_INSTALL_PREFIX`. For example, to install to user `eldridge`'s
home directory, run

~~~~~~ {.bash}
mkdir build
cd build
cmake .. -DCMAKE_INSTALL_PREFIX=/home/eldridge
make
make install
~~~~~~

`make install` creates the executables `denali` and `ctree` under
`CMAKE_INSTALL_PREFIX/bin` and all files in the directory
`CMAKE_INSTALL_PREFIX/share/denali`. To remove the software, 
simply delete these files.

## pydenali

*pydenali*, the Python package of utilities included with *denali*, is included
with *denali*, but not installed by the above steps. *pydenali* is an optional
component for working with files in *denali* format, among other things. If
you'd like to install it, see the [installation
section](../pydoc/_build/html/install.html) of the *pydenali* documentation. 
