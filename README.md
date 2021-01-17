# Fourier

Fourier is a Python 2 library for image processing. The actual library is implemented in C++. The algorithms implemented by the library are not hardware accelerated at the moment, instead there is an emphasis on simplicity of the code base

## Installing

Fourier is tested on a Linux system using Python v2.7. The library will likely compile on a Windows system with the required dependencies, however this is not tested.

To install on Linux, you will need libjpeg, libpng, Make, CMake and a C++ compiler it supports, Python 2.7 and pybind11. The library can be compiled and installed by running the following commands

``` sh
# install required dependencies
apt install build-essential \
            cmake \
            libpng-dev \
            libjpeg-dev \
            python2.7 \
            pybind11-dev
            
# clone repo
git clone git@github.com:MarkMizzi/fourier.git fourier
cd fourier
# build project Makefile in fourier/build folder (created by CMake)
cmake -Bbuild
# go to build directory and invoke make to compile and install the project.
cd build
# installs the project in the directory set by the CMake variable INSTALL_DIR (by default /usr/lib/python2.7/fourier)
make install
```
