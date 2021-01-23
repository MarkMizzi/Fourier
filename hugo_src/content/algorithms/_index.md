---
title: "Algorithms"
date: 2021-01-22T20:16:36+01:00
draft: false
level: 2
hasMath: true
---

Fourier implements a number of image processing algorithms. This section of the documentation is dedicated to explaining what makes these algorithms tick, not only in the context of Fourier, but in a general image processing scenario.

## Convolutions

Many image processing algorithms are based on the technique of convolution. In fact, Fourier's speed is, for the most part, limited by its implementation of convolutions.

If we treat a grayscale image {{< tex "I" >}} as an {{< tex "N\times M" >}} matrix, its convolution with a matrix {{< tex "K" >}} of odd width {{< tex "2 p + 1" >}} and odd height {{< tex "2 q + 1" >}} is defined as an {{< tex "N \times M" >}} matrix {{< tex "K * I" >}} such that

{{< tex display="(K * I)(i, j) = \sum_{m=0}^{2p+1}\sum_{n=0}^{2q+1}K(m, n)I(i - m + p, j + n - q)" >}}

The convolution of a grayscale image with a matrix that has even width or even height is undefined. The matrix {{< tex "K" >}} used for convolution is called a convolution kernel, or just a kernel. 

The convolution of a color image with a kernel is simply the convolution of each of its components as defined for a grayscale image.

<!-- Immediately, there appears to be a problem with the definition, because there will be points in the image which  -->
