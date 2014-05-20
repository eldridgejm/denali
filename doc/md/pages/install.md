# Installation

*denali* is available as a binary package on Windows and Linux, and can be
installed from source on nearly every modern operating system.

- [Windows](#windows)
- [Linux](#linux)
    - [From binary](#from-binary)
    - [From source](#from-source)
- [pydenali](#pydenali)


## Windows
*Denali* is available as a binary on Windows, complete with an easy-to-use
installer. For the most up-to-date package, see the [project
page](http://www.cse.ohio-state.edu/~eldridge/denali/).

On Windows, using the installer is recommended in nearly all cases,
but *denali* can also be compiled from source. The project uses the CMake
meta-compiler so that the instructions for installing from source are nearly
identical to those for Linux. If you choose to install form source on Windows,
we recommend the MinGW 4.8.2 compiler, Qt 4.8.6, and VTK 5.10.1.


## Linux

### From binary

*Denali* is available as a pre-compiled binary for several Linux distributions,
including Ubuntu. If a binary package is available for your distribution, it is
the preferred method of installation. See the [project
page](http://www.cse.ohio-state.edu/~eldridge/denali/) for an up-to-date listing
of the distributions which are supported.

*denali* installs two binaries, `denali` and `ctree`, into `/usr/bin`. It also
installs the documentation, examples, and the python module into
`/usr/share/denali`. Note that to use the python module, you must install it
from `/usr/share/denali/pydenali`, either using `pip`, or some other tool.  See
the [installation section](../pydoc/_build/html/install.html) of the *pydenali*
documentation. 

### From source

If a binary package is not available for your distribution, you can always
compile *denali* from source.  The build process is managed by CMake. You will
need Qt 4 and VTK 5 to install.  These can likely be obtained using your
system's package manager. We recommend using the gcc compiler.

To build from source and install to the default location under `/usr/local`,
invoke from the project's root directory:

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
`CMAKE_INSTALL_PREFIX/share/denali`. In particular, you may find:

- `CMAKE_INSTALL_PREFIX/share/denali/doc`: the documentation
- `CMAKE_INSTALL_PREFIX/share/denali/examples`: example files
- `CMAKE_INSTALL_PREFIX/share/denali/pydenali`: the python utility module

To remove the software, 
simply delete these files.


## pydenali

*pydenali*, the Python package of utilities included with *denali*, is included
with *denali*, but not installed by the above steps. *pydenali* is an optional
component for working with files in *denali* format, among other things. If
you'd like to install it, see the [installation
section](../pydoc/_build/html/install.html) of the *pydenali* documentation. 
