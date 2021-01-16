#!/usr/bin/env python2

import sys
sys.path.append("..") # Adds higher directory to python modules path.

from build import fourier

# read a JPEG image from a file
x = fourier.readJPEG("./lizard.jpeg")
# show some info about image
print(x)

# make a copy of the image
y = fourier.Image(x)
print(y)
# note that z = x will not copy the image, z and x will refer to the same Image:
z = x
print(z)

# transform image into a YCbCr color space.
x.to_YCbCr();
# transform image copy to grayscale color space
y.to_gray();

# write x and y to a JPEG file.
x.writeJPEG(fname="./YCbCr_lizard.jpeg", quality=100)
y.writeJPEG("./gray_lizard.jpeg", 70)

# write pixel data from x and y to a file
outfile = open("YCbCr_lizard_pixel_data.txt", "w")
outfile.write(x.dump())
outfile.close()

outfile = open("gray_lizard_pixel_data.txt", "w")
outfile.write(y.dump())
outfile.close()
