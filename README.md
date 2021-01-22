# Fourier

## Building

Fourier is tested on an Ubuntu 20.04 system using Python v2.7. The library will likely compile on Debian or Windows systems with the required dependencies, however this is not tested.

To build on Debian, you will need libjpeg, libpng, Make, CMake, g++, Python 2.7 and pybind11. The library can be compiled and installed by running the following commands

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
# create project Makefile in fourier/build
cmake -Bbuild
# go to build directory and invoke make to compile the project.
cd build 
make
```

## Usage

The test folder contains example scripts which illustrate some of the functionality of the library. 

For more details on how to use Fourier, visit https://markmizzi.github.io/fourier/
