#!/usr/bin/env python2

import sys
sys.path.append("..") # Adds higher directory to python modules path.

from build import fourier

import time

# read a JPEG image from a file
x = fourier.readJPEG("./corvette.jpeg")

s = 6.0
s_f = 4

# apply gaussian blur having standard deviation std_dev and size 2 * size_f + 1
t0 = time.time()
x.gaussian_blur(std_dev=s,
                size_f=s_f)
t1 = time.time()
print("Gaussian blur on " + str(x) +
      " having std_dev=" + str(s) + " and size " + str(2 * s_f + 1) +
      " took " + str(t1 - t0))

x.writeJPEG("./blurred_corvette.jpeg")

# read a JPEG image from a file
x = fourier.readJPEG("./eagle.jpeg")

s = 3.0
s_f = 2;

# apply gaussian blur having standard deviation std_dev and size 2 * size_f + 1
t0 = time.time()
x.gaussian_blur(std_dev=s,
                size_f=s_f)
t1 = time.time()
print("Gaussian blur on " + str(x) +
      " having std_dev=" + str(s) + " and size " + str(2 * s_f + 1) +
      " took " + str(t1 - t0) + "s")

x.writeJPEG("./blurred_eagle.jpeg")
