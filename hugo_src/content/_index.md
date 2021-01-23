---
title: "Documentation"
date: 2021-01-22T20:10:24+01:00
draft: false
---

Fourier is a simple Python2 demo library which implements a number of image processing algorithms in C++. 

The performance of the library is not a priority, instead, the main aim of the library is to offer implementations which are approachable and enhance understanding of the algorithms. To this end, all algorithms run single threaded on a CPU, rather than being offloaded to a GPU.

Here are some example images which illustrate what Fourier can be used for.

[![Gaussian blur example](./examples/fourier_example1.jpeg)](./examples/fourier_example1.jpeg)

{{< hint info >}}
**Gaussian blur**\
An image of a cathedral which has been convolved by a gaussian blur kernel. The original is shown on the left.
{{< /hint >}}

[![Edge detect example](./examples/fourier_example2.jpg)](./examples/fourier_example2.jpg)

{{< hint info >}}
**Canny edge detection**\
An image of a lizard. The edges in the image have been found using Canny edge detection, a multistage algorithm.
{{< /hint >}}

[![Compound example](./examples/fourier_example3.jpeg)](./examples/fourier_example3.jpeg)

{{< hint info >}}
**Process of Canny edge detection**\
This image illustrates part of the process of canny edge detection. The original image (top left) is first converted to grayscale and a gaussian blur is applied to it (top right). The result is then convolved with the two Sobel operators (bottom left). Finally, the image is suppressed, and then double thresholding is applied to it (bottom right).
{{< /hint >}}
