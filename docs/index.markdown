---
# Feel free to add content and custom Front Matter to this file.
# To modify the layout, see https://jekyllrb.com/docs/themes/#overriding-theme-defaults

title: "Getting started"
layout: home
nav_order: 2
---

# Getting started

This page contains a build guide for Fourier, and an overview of the functionality offered by it. For more detailed documentation, go to [Algorithms]({{ site.baseurl }}{% link algorithms/index.md %}).

## Building

Fourier is tested on an Ubuntu 20.04 system using Python v2.7. The library will likely compile on Debian or Windows systems with the required dependencies, however this is not tested.

To build on Debian, you will need libjpeg, libpng, Make, CMake, g++, Python 2.7 and pybind11. The library can be compiled and installed by running the following commands

{% highlight sh %}
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
{% endhighlight %}

## Usage

The C++ interface is wrapped in Python2 using pybind11, so as to make it easier to interact with the algorithms. The test folder contains example scripts which illustrate some of the functionality of the library. 

Most of the functionality of Fourier is contained in the class Image. An object belonging to this class represents an image which has been loaded from a file into memory.

### Reading and Writing Image files.

Fourier currently supports JPEG image files using libjpeg. These can be read from and written to using the two functions:

{% highlight python %}
# read data from an image file into the Image object x
x = fourier.readJPEG(fname="test_image.jpeg")    # the input file name
x.writeJPEG(fname="test_image_out.jpeg",         # the output file name
            quality=100)                         # the output quality
{% endhighlight %}

### Dimensions and colour space

The width and height of an image can be queried using the functions

{% highlight python %}
w = x.width()
h = x.height()
# supposing x is an Image.
{% endhighlight %}

Fourier uses arrays of floats called channels to store the image in memory. A float in a channel can take on a value between 0 and 255. There are no bounds checks, but applying an operation which causes pixels to exceed this range results in visual artifacts when writing to files. Floats were chosen rather than unsigned chars for two reasons; when applying a sequence of operations, floats give much more accurate results, and GPUs are optimized for use with floats, which means the library could more easily be adapted to use hardware acceleration in the future.

Images have an attribute called their color space which determines how many channels they have, and what each channel represents. Currently the color space of an image can be RGB, RGBX, RGBA, CMYK, YCbCr, or GRAY.

The color space of an image can be queried by calling
{% highlight python %}
c_space = x.color_space()
# supposing x is an Image.
{% endhighlight %}

## Convolutions

Fourier allows an image to be convolved with an arbitrary convolution kernel, i.e. a matrix of odd width and height. 
