## Installation

### Linux

#### From source
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
