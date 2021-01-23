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

If we treat a grayscale image {{< tex "I" >}} as an {{< tex "N\times M" >}} matrix, its convolution with a matrix {{< tex "K" >}} which is smaller than {{< tex "I">}} and has odd width {{< tex "2 p + 1" >}} and odd height {{< tex "2 q + 1" >}} is defined as an {{< tex "N \times M" >}} matrix {{< tex "K * I" >}} such that

{{< tex display="(K * I)(i, j) = \sum_{n=0}^{2p+1}\sum_{m=0}^{2q+1}K(n, m)I(i - n + p, j - m + q)" >}}

The matrix {{< tex "K" >}} used for convolution is called a convolution kernel, or just a kernel. Convolutions with kernels of even width or height, or a size larger than the image are left undefined for the sake of simplicity.

The convolution of a color image with a kernel is simply the convolution of each of its components as defined for a grayscale image.

Immediately, there appears to be a problem with the definition, because unless the kernel is {{< tex "1\times 1" >}}, there will be points in the image {{< tex "I(i, j)" >}} such that {{< tex "i - n + p < 0" >}}, {{< tex "i - n + p \geq N" >}}, {{< tex "j - m + q < 0" >}} or {{< tex "j - m + p \geq M" >}}.

There is more than one way to handle this issue. The simplest solution is to not compute the result of the convolution for pixels which are out of bound, and copy them over untouched to the convolved image. This is the approach taken by Fourier at the moment.

For small images or special kernels the above solution does not produce a satisfactory result. For Canny edge detection, for example, white lines usually appear around the image, because blurring the image at the start produces a sharp intensity gradient at the border of the image.

An alternative is to virtually "extend" the image to a new image {{< tex "I'" >}} of size {{< tex "(N + 2p) \times (M + 2q)" >}}, such that the indices of the image fall in the range {{< tex "-p < i < N + p" >}} and {{< tex "-q < j < M + q" >}}, and which is defined by

{{< tex display=`I'(i, j) = I(i', j'); \qquad
                    i' = 
                    \begin{cases}
                        i & 0 \leq i < N \\
                        0 & i < 0 \\
                        N - 1 & i \geq N
                    \end{cases}
                    \qquad
                    j' = 
                    \begin{cases}
                        j & 0 \leq j < M \\
                        0 & j < 0 \\
                        M - 1 & j \geq M
                    \end{cases}` >}}
                     
It should be clear that {{< tex "I'" >}} is well defined in terms of {{< tex "I" >}}, and one can then redefine the convolution as

{{< tex display="(K * I)(i, j) = \sum_{n=0}^{2p+1}\sum_{m=0}^{2q+1}K(n, m)I'(i - n + p, j - m + q)" >}}

This new definition works for any {{< tex "0 \leq i < N" >}} and {{< tex "0 \leq j < M" >}}.

In fact most strategies which handle the issue of convolution at the border of the image are based on this tactic of finding a new image {{< tex "I'" >}} which is well defined in terms of {{< tex "I" >}}, but which is larger to account for the convolution.

Another such strategy is to "mirror" the image:
{{< tex display=`I'(i, j) = I(i', j'); \qquad
                    i' = 
                    \begin{cases}
                        i & 0 \leq i < N \\
                        -i & i < 0 \\
                        2N - i & i \geq N
                    \end{cases}
                    \qquad
                    j' = 
                    \begin{cases}
                        j & 0 \leq j < M \\
                        -j & j < 0 \\
                        2M - j & j \geq M
                    \end{cases}` >}}
