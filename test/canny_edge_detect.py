#!/usr/bin/env python2

import sys
sys.path.append("..") # Adds higher directory to python modules path.

from build import fourier

import time

# read a JPEG image from a file
x = fourier.readJPEG("./lizard.jpeg")
# detect edges:
# defaults of algorithm are:
#        * blur_std_dev = 1.4f
#        * blur_size_f = 2
#        * upper_threshold = 76.8
#        * lower_threshold = 25.6
t0 = time.time()
x.canny_edge_detect()
t1 = time.time()
print("Canny edge detection on " + str(x) +
      " having blur_std_dev = 1.4, blur_size_f = 2," +
      " upper_threshold = 76.8 and lower_threshold = 25.6" +
       " took " + str(t1 - t0))

x.writeJPEG("./lizard_edges.jpeg")

x = fourier.readJPEG("./corvette.jpeg")

b_s = 3.0
b_s_f = 3
u_t = 50.0
l_t = 18.0
t0 = time.time()
x.canny_edge_detect(blur_std_dev=b_s,
                    blur_size_f=b_s_f,
                    upper_threshold=u_t,
                    lower_threshold=l_t)
t1 = time.time()
print("Canny edge detection on " + str(x) +
      " having blur_std_dev = " + str(b_s) +
      ", blur_size_f = " + str(b_s_f) +
      ", upper_threshold = " + str(u_t) +
      " and lower_threshold = " + str(l_t) +
       " took " + str(t1 - t0))

x.writeJPEG("./corvette_edges.jpeg")

x = fourier.readJPEG("./jag.jpeg")

b_s = 1.4
b_s_f = 2
u_t = 76.8
l_t = 30.6
t0 = time.time()
x.canny_edge_detect(blur_std_dev=b_s,
                    blur_size_f=b_s_f,
                    upper_threshold=u_t,
                    lower_threshold=l_t)
t1 = time.time()
print("Canny edge detection on " + str(x) +
      " having blur_std_dev = " + str(b_s) +
      ", blur_size_f = " + str(b_s_f) +
      ", upper_threshold = " + str(u_t) +
      " and lower_threshold = " + str(l_t) +
       " took " + str(t1 - t0))

x.writeJPEG("./jag_edges.jpeg")
