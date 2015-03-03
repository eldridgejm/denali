# Installation

*Denali* is available as a binary package on Windows, Debian/Ubuntu Linux, and
Mac OS X. It can also be compiled from source on any platform supporting Qt 4
and VTK 5 or newer.

- [Windows](#windows)
- [OS X](#os-x)
- [Linux](#linux)
- [From source](#from-source)

## Windows
*See the [project page](http://denali.cse.ohio-state.edu) for the latest version
of the Denali installer.*

The recommended method of installing *Denali* on Windows is via the binary
installer. 

1. Download and launch the installer.
2. Click "I Agree" to accept the license.
3. Click "Install" to install to the default location.

This will typically install *Denali* to `C:\Program Files (x86)\denali`. You can
find the examples and a copy of the documentation under this directory, as well
as [*ctree*](./ctree.html) and [*pydenali*](../pydoc/_build/html/index.html), a
python package for working with *denali* file formats.

You can launch *Denali* from the Start Menu.

## OS X
*See the [project page](http://denali.cse.ohio-state.edu) for the latest version
of the Denali installer.*

The recommended method of installing *Denali* on Mac OS X is by downloading the
`.dmg` found on the project page.

1. Download and double click the `denali_x.y.dmg` to mount the image.
2. Click and drag the *Denali* app icon to your *Applications* folder.

The OS X `.dmg` distribution **does not** include *pydenali* or the examples.
You can download these separately from the project page.

[`ctree`](./ctree.html) *is* included, however. You can access it from the
command line. If you've installed *Denali* as described above, you can find
`ctree` at `/Applications/Denali.app/Contents/MacOS/ctree`.

## Linux
*See the [project page](http://denali.cse.ohio-state.edu) for the latest version
of the Denali installer.*

The preferred method of installing *Denali* on Ubuntu or Debian is to download
the `.deb` file on the project page, and install using `dpkg -i denali_x.y.deb`.

After installation, you can find the examples, documentation, and python package
in `/usr/share/denali`.

For distributions that do not use the `.deb` format, it is recommended to
install *Denali* from source. *Denali* was developed on linux, and tested to
worked with the Ubuntu, Debian, Red Hat, and Arch distributions.

## From source

*Denali* can be installed from source on nearly any modern operating system.
The build process is managed by the [CMake](http://www.cmake.org/)
cross-platform meta-compiler. The build instructions are nearly identical
across operating systems.

[Download or clone the source from GitHub](https://github.com/eldridgejm/denali).

### Prerequisites

*Denali* depends on the following libraries:

- [Qt 4](http://qt-project.org/)
- [VTK 5](http://www.vtk.org/)
- [Boost](http://www.boost.org/)

Furthermore, you must download and install the [CMake](http://www.cmake.org/)
meta-compiler in order to build *denali* from source.

These libraries must be compiled or installed prior to building *denali*. If you
choose to use your operating system's package manager to install these libraries,
be sure to also download and install the appropriate header file packages
(these packages are often suffixed with `-dev` in Linux distributions).

VTK must be installed with Qt support. It is often the case that the versions
of VTK available in your package manager's repositories are *not* compiled with
Qt support. Trying to compile *denali* against a version of VTK which was not
compiled with Qt support will result in missing file errors during the build
process, such as `QVTKWidget.h not found`.  In this case, it is recommended to
build VTK from source using CMake with the cache entries `VTK_USE_QT` and
`BUILD_SHARED_LIBS` set.

It is difficult to give an exact set of instructions which works for every
user's system.  Nevertheless, the following steps were found to work on the
developer's machines:

#### Mac OS X

- Install Qt via brew: `brew install qt`
- Install boost via brew: `brew install boost`
- Download and compile VTK from source using CMake with the `BUILD_SHARED_LIBS` and
  `VTK_USE_QT` options set.

#### Windows

Compiling from source on Windows is somewhat complicated and therefore detailed instructions
are not provided here. If you *do* choose to install from source on Windows, it is recommended
that you specifically use the MinGW 4.8.2 compiler, Qt 4.8.6, and VTK 5.10.1.

### Compiling *denali*

Once the prerequisites are installed, compiling *denali* is straightforward. Download
and extract *denali*, and navigate to the project's root. Then invoke:

    # inside the denali source tree...
    mkdir build && cd build
    cmake ..
    make

and optionally, if you wish to install to a system-wide location:

    sudo make install

By default this installs *denali* to the various directories under `/usr/local` on Unix-like
systems, and the `Program Files` directory under Windows. These locations can be changed by specifying
the `CMAKE_INSTALL_PREFIX` when invoking `cmake`.

`make install` creates the executables `denali` and `ctree` under
`CMAKE_INSTALL_PREFIX/bin` and all files in the directory
`CMAKE_INSTALL_PREFIX/share/denali`. In particular, you may find:

- `CMAKE_INSTALL_PREFIX/share/denali/doc`: the documentation
- `CMAKE_INSTALL_PREFIX/share/denali/examples`: example files
- `CMAKE_INSTALL_PREFIX/share/denali/pydenali`: the python utility module

To remove the software, 
simply delete these files.

<div style="text-align: center; margin-top: 20px">
<a href="./intro.html" style="font-size: 20px">Previous: Introduction</a>
<span style="font-size: 20px; opacity: .5"> | </span>
<a href="./tutorial.html" style="font-size: 20px">Next: Tutorial</a>
</div>
