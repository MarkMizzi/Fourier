---
title: "Canny Edge Detection"
date: 2021-01-23T16:15:01+01:00
draft: false 
hasMath: true
---

There are many applications which require a fast edge detection algorithm. For example, real time object detection algorithms apply edge detection on the visual data fed to them in order to simplify the object detection. 

Despite being developed in 1986, the Canny edge detection algorithm is still at the forefront of edge detection methods. This is because it offers a very good trade-off between accuracy and speed.

## Definition of an edge

An edge is a curve in the image of roughly even intensity, such that in crossing the curve there is an abrupt change in intensity. We expect to find edges where one object ends and another begins, or at places where a sharp shadow is cast by an object.

<!-- mathematically define an edge -->

<!--Several natural phenomena such as diffraction can blur edges, making -->

## The Canny Edge Detector

The Canny Edge Detector follows a 5 step process which will now be outlined. To help the discussion the following image will be passed through each step of the algorithm as implemented in Fourier.

[![Alt: Lion fish (Image not loaded)](./fish.jpeg)](./fish.jpeg)

First, the image is converted to grayscale, and a gaussian blur is applied to it. The blurring is necessary because the next step uses differentiation operators, which are very sensitive to noise.

The Gaussian blur removes a large amount of noise from the image, but also defocuses the edges. The blur parameters must hence be adjusted for optimal performance.

![Alt: Gray lion fish (Image not loaded)](./fish_gray.jpeg)

We are interested in places in the image {{< tex "I" >}} where {{< tex "|\nabla I|" >}} is large, as this marks pixels where the intensity changes abruptly. For the next step, we will also need the direction {{< tex "\theta" >}} of {{< tex "\nabla I" >}}.

The Sobel kernels, defined as

{{< tex display=`S_x := 
                    \begin{pmatrix}
                        1 & 0 & -1 \\
                        2 & 0 & -2 \\
                        1 & 0 & -1
                    \end{pmatrix}; 
                    \qquad
                S_y := 
                    \begin{pmatrix}
                        1 & 2 & 1 \\
                        0 & 0 & 0 \\
                        -1 & -2 & -1
                    \end{pmatrix}` >}}

are such that {{< tex `S_x * I \approx \frac{\partial I}{\partial x}` >}} and {{< tex `S_y * I \approx \frac{\partial I}{\partial y}` >}}

Hence

{{< tex display="|\nabla I| = \sqrt{(S_x * I)^2 + (S_y * I)^2}" >}}
{{< tex display="\theta = \arctan \bigg(\frac{S_y * I}{S_x * I}\bigg)" >}}

These definitions give us an easy and quick way to find what we need.

![Alt: Lion fish sobel (Image not loaded)](./fish_sobel.jpeg)
