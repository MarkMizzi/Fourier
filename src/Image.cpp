#include "Image.hpp"

#include <cstdio>
#include <cerrno>
#include <cmath>
#include <jpeglib.h>
#include <string>
#include <sstream>
#include <fstream>
#include <array>
#include <system_error>
#include "Kernel.hpp"

const std::map<ChannelType, std::array<float, 4>> RGB_to_YCbCr {
{
    INTENSITY, {
    65.738 / 256.0,
    129.057 / 256.0,
    25.064 / 256.0,
    16.0,
    },
}, {
    Cb, {
    -37.945 / 256.0,
    -74.494 / 256.0,
    112.439 / 256.0,
    128.0,
    },
}, {
    Cr, {
    112.439 / 256.0,
    -94.154 / 256.0,
    -18.285 / 256.0,
    128.0,
    },
}
};

const std::map<ChannelType, std::array<float, 4>> YCbCr_to_RGB {
{
    RED, {
    298.082 / 256.0,
    0.0,
    408.583 / 256.0,
    -222.921,
    },
}, {
    GREEN, {
    298.082 / 256.0,
    -100.291 / 256.0,
    -208.120 / 256.0,
    135.576,
    },
}, {
    BLUE, {
    298.082 / 256.0,
    516.412 / 256.0,
    0.0,
    -276.836,
    },
}
};

void
Image::to_RGB()
{
    // if image is already RGB, RGBX, or RGBA, minimal changes have to be made.
    switch (colorSpace()) {
        case RGB:
            return;
        case RGBX:
        {
            image_data.erase(ALPHA_IGNORED);
            c_space = RGB;
            return;
        }
        case RGBA:
        {
            image_data.erase(ALPHA);
            c_space = RGB;
            return;
        }
        default:
            break;
    }

    image_data.insert({ RED, std::vector<float>(width() * height(), 0) });
    image_data.insert({ GREEN, std::vector<float>(width() * height(), 0) });
    image_data.insert({ BLUE, std::vector<float>(width() * height(), 0) });

    switch (colorSpace()) {
        case RGB:
        case RGBX:
        case RGBA:
            throw std::logic_error("This color space should have been handled earlier.");
        case CMYK:
            for (ssize_t i = 0; i < width(); i++)
                for (ssize_t j = 0; j < height(); j++) {
                    (*this)(RED, i, j) = (1 - get(CYAN, i, j)) * (1 - get(BLACK, i, j)) / 256.0;
                    (*this)(GREEN, i, j) = (1 - get(MAGENTA, i, j)) * (1 - get(BLACK, i, j)) / 256.0;
                    (*this)(BLUE, i, j) = (1 - get(YELLOW, i, j)) * (1 - get(BLACK, i, j)) / 256.0;
                };
            image_data.erase(CYAN);
            image_data.erase(MAGENTA);
            image_data.erase(YELLOW);
            image_data.erase(BLACK);
            break;
        case YCbCr:
            for (ssize_t i = 0; i < width(); i++)
                for (ssize_t j = 0; j < height(); j++) {
                    (*this)(RED, i, j) = YCbCr_to_RGB.at(RED)[0] * get(INTENSITY, i, j) +
                                         YCbCr_to_RGB.at(RED)[1] * get(Cb, i, j) +
                                         YCbCr_to_RGB.at(RED)[2] * get(Cr, i, j) +
                                         YCbCr_to_RGB.at(RED)[3];
                    (*this)(GREEN, i, j) = YCbCr_to_RGB.at(GREEN)[0] * get(INTENSITY, i, j) +
                                           YCbCr_to_RGB.at(GREEN)[1] * get(Cb, i, j) +
                                           YCbCr_to_RGB.at(GREEN)[2] * get(Cr, i, j) +
                                           YCbCr_to_RGB.at(GREEN)[3];
                    (*this)(BLUE, i, j) = YCbCr_to_RGB.at(BLUE)[0] * get(INTENSITY, i, j) +
                                          YCbCr_to_RGB.at(BLUE)[1] * get(Cb, i, j) +
                                          YCbCr_to_RGB.at(BLUE)[2] * get(Cr, i, j) +
                                          YCbCr_to_RGB.at(BLUE)[3];
                    };
            image_data.erase(INTENSITY);
            image_data.erase(Cb);
            image_data.erase(Cr);
            break;
        case GRAY:
            for (ssize_t i = 0; i < width(); i++)
                for (ssize_t j = 0; j < height(); j++) {
                    (*this)(RED, i, j) = YCbCr_to_RGB.at(RED)[0] * get(INTENSITY, i, j) +
                                         YCbCr_to_RGB.at(RED)[3];
                    (*this)(GREEN, i, j) = YCbCr_to_RGB.at(GREEN)[0] * get(INTENSITY, i, j) +
                                           YCbCr_to_RGB.at(GREEN)[3];
                    (*this)(BLUE, i, j) = YCbCr_to_RGB.at(BLUE)[0] * get(INTENSITY, i, j) +
                                          YCbCr_to_RGB.at(BLUE)[3];
                };
            image_data.erase(INTENSITY);
            break;
    }
    c_space = RGB;
}

void
Image::to_YCbCr()
{
    if (colorSpace() == YCbCr)
        return;
    else if (colorSpace() == GRAY) {
        image_data[Cb] = std::vector<float>(width() * height(), 0);
        image_data[Cr] = std::vector<float>(width() * height(), 0);
        c_space = YCbCr;
        return;
    }

    image_data.insert({ INTENSITY, std::vector<float>(width() * height(), 0) });
    image_data.insert({ Cb, std::vector<float>(width() * height(), 0) });
    image_data.insert({ Cr, std::vector<float>(width() * height(), 0) });

    switch (colorSpace()) {
        case RGBX:
            image_data.erase(ALPHA_IGNORED);
            goto RGB;
            break;
        case RGBA:
            image_data.erase(ALPHA);
            goto RGB;
            break;
        case RGB:
        RGB:
            for (ssize_t i = 0; i < width(); i++)
                for (ssize_t j = 0; j < height(); j++) {
                    (*this)(INTENSITY, i, j) = RGB_to_YCbCr.at(INTENSITY)[0] * get(RED, i, j) +
                                               RGB_to_YCbCr.at(INTENSITY)[1] * get(GREEN, i, j) +
                                               RGB_to_YCbCr.at(INTENSITY)[2] * get(BLUE, i, j) +
                                               RGB_to_YCbCr.at(INTENSITY)[3];
                    (*this)(Cb, i, j) = RGB_to_YCbCr.at(Cb)[0] * get(RED, i, j) +
                                        RGB_to_YCbCr.at(Cb)[1] * get(GREEN, i, j) +
                                        RGB_to_YCbCr.at(Cb)[2] * get(BLUE, i, j) +
                                        RGB_to_YCbCr.at(Cb)[3];
                    (*this)(Cr, i, j) = RGB_to_YCbCr.at(Cr)[0] * get(RED, i, j) +
                                        RGB_to_YCbCr.at(Cr)[1] * get(GREEN, i, j) +
                                        RGB_to_YCbCr.at(Cr)[2] * get(BLUE, i, j) +
                                        RGB_to_YCbCr.at(Cr)[3];
                };
            image_data.erase(RED);
            image_data.erase(GREEN);
            image_data.erase(BLUE);
            break;
        case CMYK:
            for (ssize_t i = 0; i < width(); i++)
                for (ssize_t j = 0; j < height(); j++) {
                    float red = (1 - get(CYAN, i, j)) * (1 - get(BLACK, i, j)) / 256.0;
                    float green = (1 - get(MAGENTA, i, j)) * (1 - get(BLACK, i, j)) / 256.0;
                    float blue = (1 - get(YELLOW, i, j)) * (1 - get(BLACK, i, j)) / 256.0;
                    
                    (*this)(INTENSITY, i, j) = RGB_to_YCbCr.at(INTENSITY)[0] * red +
                                               RGB_to_YCbCr.at(INTENSITY)[1] * green +
                                               RGB_to_YCbCr.at(INTENSITY)[2] * blue +
                                               RGB_to_YCbCr.at(INTENSITY)[3];
                    (*this)(Cb, i, j) = RGB_to_YCbCr.at(Cb)[0] * red +
                                        RGB_to_YCbCr.at(Cb)[1] * green +
                                        RGB_to_YCbCr.at(Cb)[2] * blue +
                                        RGB_to_YCbCr.at(Cb)[3];
                    (*this)(Cr, i, j) = RGB_to_YCbCr.at(Cr)[0] * red +
                                        RGB_to_YCbCr.at(Cr)[1] * green +
                                        RGB_to_YCbCr.at(Cr)[2] * blue +
                                        RGB_to_YCbCr.at(Cr)[3];                   
                    };
            image_data.erase(CYAN);
            image_data.erase(MAGENTA);
            image_data.erase(YELLOW);
            image_data.erase(BLACK);
            break;
        case YCbCr:
        case GRAY:
            throw std::logic_error("This color space should have been handled earlier.");                   
    }
    c_space = YCbCr;
}

void
Image::to_gray()
{
    if (colorSpace() == GRAY)
        return;
    else if (colorSpace() == YCbCr) {
        image_data.erase(Cb);
        image_data.erase(Cr);
        c_space = GRAY;
        return;
    }

    image_data.insert({ INTENSITY, std::vector<float>(width() * height(), 0) });

    switch (colorSpace()) {
        case RGB:
        case RGBX:
        case RGBA:
            for (ssize_t i = 0; i < width(); i++)
                for (ssize_t j = 0; j < height(); j++) {
                   (*this)(INTENSITY, i, j) = RGB_to_YCbCr.at(INTENSITY)[0] * get(RED, i, j) +
                                              RGB_to_YCbCr.at(INTENSITY)[1] * get(GREEN, i, j) +
                                              RGB_to_YCbCr.at(INTENSITY)[2] * get(BLUE, i, j) +
                                              RGB_to_YCbCr.at(INTENSITY)[3];
                };
            image_data.erase(RED);
            image_data.erase(GREEN);
            image_data.erase(BLUE);
            break;
        case CMYK:
            for (ssize_t i = 0; i < width(); i++)
                for (ssize_t j = 0; j < height(); j++) {
                    float red = (1 - get(CYAN, i, j)) * (1 - get(BLACK, i, j)) / 256.0;
                    float green = (1 - get(MAGENTA, i, j)) * (1 - get(BLACK, i, j)) / 256.0;
                    float blue = (1 - get(YELLOW, i, j)) * (1 - get(BLACK, i, j)) / 256.0;

                    (*this)(INTENSITY, i, j) = RGB_to_YCbCr.at(INTENSITY)[0] * red +
                                               RGB_to_YCbCr.at(INTENSITY)[1] * green +
                                               RGB_to_YCbCr.at(INTENSITY)[2] * blue +
                                               RGB_to_YCbCr.at(INTENSITY)[3];
                };
            image_data.erase(CYAN);
            image_data.erase(MAGENTA);
            image_data.erase(YELLOW);
            image_data.erase(BLACK);
            break;
        case YCbCr:
        case GRAY:
            throw std::logic_error("This color space should have been handled earlier.");                   
    }
    c_space = GRAY;
}

inline
void
Image::convolve_component(ChannelType ch,
                          const Kernel& kern)
{
    std::vector<float> convolved_comp(width() * height(), 0);

    ssize_t kern_h_f = (kern.size() - 1) / 2;
    ssize_t kern_w_f = (kern[0].size() - 1) / 2;

    for (ssize_t i = kern_w_f; i < width() - kern_w_f; i++) {
        // elements (i, j) which are in the safe zone; i.e. convolution at these pixels does not cause issues
        for (ssize_t j = kern_h_f; j < height() - kern_h_f; j++) {
            for (size_t m = 0; m < kern[0].size(); m++)
                for (size_t n = 0; n < kern.size(); n++)
                    convolved_comp[width() * j + i] += get(ch,
                                                           i + m - kern_w_f,
                                                           j + n - kern_h_f) * kern[n][m];
        }
    }

    for (ssize_t i = kern_w_f; i < width() - kern_w_f; i++) {
        for (ssize_t j = kern_h_f; j < height() - kern_h_f; j++) {
            (*this)(ch, i, j) = convolved_comp[width() * j + i];
        }
    }
}


Image&
Image::operator*(const Kernel& kern)
{
    // check that the kernel's rows are all the same size, and that its height and width is odd.
    if (kern.size() % 2 == 0)
        throw std::invalid_argument("Kernel height must be odd");

    size_t kern_w = kern[0].size();
    for (auto it = kern.begin();
         it != kern.end();
         ++it) {
        size_t row_w = (*it).size();
        if (row_w % 2 == 0)
            throw std::invalid_argument("Kernel width must be odd");
        if (row_w != kern_w)
            throw std::invalid_argument("Kernel rows must be the same size");
    }

    switch (colorSpace()) {
        case RGB:
        case RGBA:
        case RGBX:
            convolve_component(RED, kern);
            convolve_component(GREEN, kern);
            convolve_component(BLUE, kern);
            break;
        case CMYK:
            convolve_component(CYAN, kern);
            convolve_component(MAGENTA, kern);
            convolve_component(YELLOW, kern);
            convolve_component(BLACK, kern);
            break;
        case YCbCr:
            convolve_component(INTENSITY, kern);
            // convolve_component(Cb, kern);
            // convolve_component(Cr, kern);
            break;
        case GRAY:
            convolve_component(INTENSITY, kern);
            break;
    }

    return *this;
}

// if a blur kernel's size is less than 2 * BLUR_ACC * std_dev + 1, it is normalized
#define BLUR_ACC 3

Image&
Image::gaussian_blur_naive(float std_dev,
                           ssize_t kern_size_f)
{
    GaussianKernel k(std_dev, kern_size_f);

    if (kern_size_f < BLUR_ACC * std_dev)
        k.normalize();

    return (*this) * k;
}

Image&
Image::gaussian_blur(float std_dev,
                     ssize_t kern_size_f)
{
    GaussianRow r(std_dev, kern_size_f);
    GaussianColumn c(std_dev, kern_size_f);

    if (kern_size_f < BLUR_ACC * std_dev) {
        r.normalize();
        c.normalize();
    }

    return (*this) * r * c;
}

#undef BLUR_ACC

Image&
Image::box_blur(ssize_t kern_size_f)
{
    Kernel row_k(1,
                 KernelRow(2 * kern_size_f + 1,
                           1.0f / (2 * kern_size_f + 1)));
    Kernel column_k(2 * kern_size_f + 1,
                    KernelRow(1,
                              1.0f / (2 * kern_size_f + 1)));
    return (*this) * row_k * column_k;
}

Image&
Image::canny_edge_detect(float blur_std_dev,
                         ssize_t blur_size_f,
                         float upper_threshold,
                         float lower_threshold)
{
    to_gray();

    gaussian_blur(blur_std_dev, blur_size_f);

#define SOBEL_X {{1.0f, 0.0f, -1.0f}, \
                 {2.0f, 0.0f, -2.0f}, \
                 {1.0f, 0.0f, -1.0f}}
#define SOBEL_Y {{1.0f, 2.0f, 1.0f}, \
                 {0.0f, 0.0f, 0.0f}, \
                 {-1.0f, -2.0f, -1.0f}}

#define SOBEL_FELDMAN_X {{3.0f, 0.0f, -3.0f},   \
                         {10.0f, 0.0f, -10.0f},  \
                         {3.0f, 0.0f, -3.0f}}
#define SOBEL_FELDMAN_Y {{3.0f, 10.0f, 3.0f},    \
                         {0.0f, 0.0f, 0.0f},     \
                         {-3.0f, -10.0f, -3.0f}}

#define SCHARR_X {{47.0f, 0.0f, -47.0f},        \
                  {162.0f, 0.0f, -162.0f},      \
                  {47.0f, 0.0f, -47.0f}}
#define SCHARR_Y {{47.0f, 162.0f, 47.0f},       \
                  {0.0f, 0.0f, 0.0f},           \
                  {-47.0f, -162.0f, -47.0f}}

#define SOBEL_X_LARGE {{-5.0f / 20, -4.0f / 20, 0.0f, 4.0f / 20, 5.0f / 20},   \
                       {-8.0f / 20, -10.0f / 20, 0.0f, 10.0f / 20, 8.0f / 20}, \
                       {-10.0f / 20, -20.0f / 20, 0.0f, 20.0f / 20, 10.0f / 20}, \
                       {-8.0f / 20, -10.0f / 20, 0.0f, 10.0f / 20, 8.0f / 20},   \
                       {-5.0f / 20, -4.0f / 20, 0.0f, 4.0f / 20, 5.0f / 20}}
#define SOBEL_Y_LARGE {{-5.0f / 20, -8.0f / 20, -10.0f / 20, -8.0f / 20, -5.0f / 20}, \
                       {-4.0f / 20, -10.0f / 20, -20.0f / 20, -10.0f / 20, -4.0f / 20}, \
                       {0.0f, 0.0f, 0.0f, 0.0f, 0.0f},         \
                       {4.0f / 20, 10.0f / 20, 20.0f / 20, 10.0f / 20, 4.0f / 20},      \
                       {5.0f / 20, 8.0f / 20, 10.0f / 20, 8.0f / 20, 5.0f / 20}}

    Kernel x_edge_k SOBEL_X;
    Kernel y_edge_k SOBEL_Y;

#undef SOBEL_X
#undef SOBEL_Y

#undef SOBEL_FELDMAN_X
#undef SOBEL_FELDMAN_Y

#undef SCHARR_X
#undef SCHARR_Y

#undef SOBEL_X_LARGE
#undef SOBEL_Y_LARGE

    Image tmp_copy(*this);

    // find an approximation of gradient direction
    Image Theta(atan2(pow(tmp_copy * y_edge_k, 2),
                      pow(*this * x_edge_k, 2)));
    // find an approximation of image gradient.
    *this = sqrt((*this = *this + tmp_copy)) * (1.0f / sqrt(2.0f));

    // non-maximum suppression:
    //    Pixels which are on an edge are analyzed as follows.
    //      The approximate value of a pixel on either side of the edge in the gradient direction is calculated,
    //         if both are brighter than the center pixel, this is set to 0.
    std::vector<float> tmp(image_data[INTENSITY]); // temp store while image is suppressed
    for (ssize_t i = 1; i < width() - 1; i++)
        for (ssize_t j = 1; j < height() - 1; j++) {
            float t = Theta.get(INTENSITY, i, j);
            float next_pixel = 0; // variable to store value of next pixel on the edge
            float last_pixel = 0; // variable to store value of last pixel on the edge

            // find two pixels i_0 and i_1 such that the direction t of the image's gradient is between them
            // The two pixels have directions t_0 and t_1 respectively which are multiples of pi / 4 radians.
            // Using linear interpolation, the value of an imaginary pixel in the direction of the gradient is then
            //  i = ((t_1 - t)i_1 + (t - t_0)i_0) / (t_1 - t_0)
            //  Similarly, we can find the value of an imaginary pixel in the direction opposite the that of the gradient.
            //  This is how values for next_pixel and last_pixel are found.
#define INTERVAL (M_PI / 4)
           // if (t >= 0 && t < INTERVAL) {
           //     next_pixel = ((INTERVAL - t) * get(INTENSITY, i + 1, j - 1) -
           //                   t * get(INTENSITY, i + 1, j)) / INTERVAL;
           //     last_pixel = ((INTERVAL - t) * get(INTENSITY, i - 1, j + 1) -
           //                   t * get(INTENSITY, i - 1, j)) / INTERVAL;
           // } else if (t >= INTERVAL && t < 2 * INTERVAL) {
           //     next_pixel = ((2 * INTERVAL - t) * get(INTENSITY, i, j - 1) -
           //                   (INTERVAL - t) * get(INTENSITY, i + 1, j - 1)) / INTERVAL;
           //     last_pixel = ((2 * INTERVAL - t) * get(INTENSITY, i, j + 1) -
           //                   (INTERVAL - t) * get(INTENSITY, i - 1, j + 1)) / INTERVAL;
           // } else if (t >= 2 * INTERVAL && t < 3 * INTERVAL) {
           //     next_pixel = ((3 * INTERVAL - t) * get(INTENSITY, i - 1, j - 1) -
           //                   (2 * INTERVAL - t) * get(INTENSITY, i, j - 1)) / INTERVAL;
           //     last_pixel = ((3 * INTERVAL - t) * get(INTENSITY, i + 1, j + 1) -
           //                   (2 * INTERVAL - t) * get(INTENSITY, i, j + 1)) / INTERVAL;
           // } else if (t >= 3 * INTERVAL && t < 4 * INTERVAL) {
           //     next_pixel = ((4 * INTERVAL - t) * get(INTENSITY, i - 1, j) -
           //                   (3 * INTERVAL - t) * get(INTENSITY, i - 1, j - 1)) / INTERVAL;
           //     last_pixel = ((4 * INTERVAL - t) * get(INTENSITY, i + 1, j) -
           //                   (3 * INTERVAL - t) * get(INTENSITY, i + 1, j + 1)) / INTERVAL;
           // } else if (t >= 4 * INTERVAL && t < 5 * INTERVAL) {
           //     next_pixel = ((5 * INTERVAL - t) * get(INTENSITY, i - 1, j + 1) -
           //                   (4 * INTERVAL - t) * get(INTENSITY, i - 1, j)) / INTERVAL;
           //     last_pixel = ((5 * INTERVAL - t) * get(INTENSITY, i + 1, j - 1) -
           //                   (4 * INTERVAL - t) * get(INTENSITY, i + 1, j)) / INTERVAL;
           // } else if (t >= 5 * INTERVAL && t < 6 * INTERVAL) {
           //     next_pixel = ((6 * INTERVAL - t) * get(INTENSITY, i, j + 1) -
           //                   (5 * INTERVAL - t) * get(INTENSITY, i - 1, j + 1)) / INTERVAL;
           //     last_pixel = ((6 * INTERVAL - t) * get(INTENSITY, i, j - 1) -
           //                   (5 * INTERVAL - t) * get(INTENSITY, i + 1, j - 1)) / INTERVAL;
           // } else if (t >= 6 * INTERVAL && t < 7 * INTERVAL) {
           //     next_pixel = ((7 * INTERVAL - t) * get(INTENSITY, i + 1, j + 1) -
           //                   (6 * INTERVAL - t) * get(INTENSITY, i, j + 1)) / INTERVAL;
           //     last_pixel = ((7 * INTERVAL - t) * get(INTENSITY, i - 1, j - 1) -
           //                   (6 * INTERVAL - t) * get(INTENSITY, i, j - 1)) / INTERVAL;
           // } else if (t >= 7 * INTERVAL && t < 8 * INTERVAL) {
           //     next_pixel = ((8 * INTERVAL - t) * get(INTENSITY, i + 1, j) -
           //                   (7 * INTERVAL - t) * get(INTENSITY, i + 1, j + 1)) / INTERVAL;
           //     last_pixel = ((8 * INTERVAL - t) * get(INTENSITY, i - 1, j) -
           //                   (7 * INTERVAL - t) * get(INTENSITY, i - 1, j - 1)) / INTERVAL;
           // }

           if (t >= 0 && t < INTERVAL) {
               next_pixel = get(INTENSITY, i + 1, j);
               last_pixel = get(INTENSITY, i - 1, j);
           } else if (t >= INTERVAL && t < 2 * INTERVAL) {
               next_pixel = get(INTENSITY, i + 1, j - 1);
               last_pixel = get(INTENSITY, i - 1, j + 1);
           } else if (t >= 2 * INTERVAL && t < 3 * INTERVAL) {
               next_pixel = get(INTENSITY, i, j - 1);
               last_pixel = get(INTENSITY, i, j + 1);
           } else if (t >= 3 * INTERVAL && t < 4 * INTERVAL) {
               next_pixel = get(INTENSITY, i - 1, j - 1);
               last_pixel = get(INTENSITY, i + 1, j + 1);
           } else if (t >= 4 * INTERVAL && t < 5 * INTERVAL) {
               next_pixel = get(INTENSITY, i - 1, j);
               last_pixel = get(INTENSITY, i + 1, j);
           } else if (t >= 5 * INTERVAL && t < 6 * INTERVAL) {
               next_pixel = get(INTENSITY, i - 1, j + 1);
               last_pixel = get(INTENSITY, i + 1, j - 1);
           } else if (t >= 6 * INTERVAL && t < 7 * INTERVAL) {
               next_pixel = get(INTENSITY, i, j + 1);
               last_pixel = get(INTENSITY, i, j - 1);
           } else if (t >= 7 * INTERVAL && t < 8 * INTERVAL) {
               next_pixel = get(INTENSITY, i + 1, j + 1);
               last_pixel = get(INTENSITY, i - 1, j - 1);
           }

#undef INTERVAL

            if (get(INTENSITY, i, j) < last_pixel ||
                get(INTENSITY, i, j) < next_pixel)
                tmp[width() * j + i] = 0;
        }
    image_data[INTENSITY] = tmp;

    // double threshold: pixels above upper_threshold are set to maximum intensity.
    //                   pixels above lower_threshold but below upper_threshold are set to half intensity.
    //                   Anything else is blacked out.
    for (ssize_t i = 0; i < width(); i++)
        for (ssize_t j = 0; j < height(); j++) {
            if (get(INTENSITY, i, j) >= upper_threshold)
                (*this)(INTENSITY, i, j) = get_max_intensity();
            else if (get(INTENSITY, i, j) >= lower_threshold)
                (*this)(INTENSITY, i, j) = get_max_intensity() / 2;
            else
                (*this)(INTENSITY, i, j) = 0;
        }

    return *this;
}

// IMPLEMENT frei chen edge detection

// #define FREI_CHEN_1 {{1.0f / (2.0f * sqrt(2.0f)), 0.5f, 1.0f / (2.0f * sqrt(2.0f))}, \
//                      {0.0f, 0.0f, 0.0f},                                             \
//                      {-1.0f / (2.0f * sqrt(2.0f)), -0.5f, -1.0f / (2.0f * sqrt(2.0f))}}


// #define FREI_CHEN_2 {{1.0f / (2.0f * sqrt(2.0f)), 0.0f, -1.0f / (2.0f * sqrt(2.0f))}, \
//                      {0.5f, 0.0f, -0.5f},                                             \
//                      {1.0f / (2.0f * sqrt(2.0f)), 0.0f, -1.0f / (2.0f * sqrt(2.0f))}}

// #define FREI_CHEN_3 {{0.0f, -1.0f / (2.0f * sqrt(2.0f)), 0.5f},                       \
//                      {1.0f / (2.0f * sqrt(2.0f)), 0.0f, -1.0f / (2.0f * sqrt(2.0f))}, \
//                      {-0.5f, 1.0f / (2.0f * sqrt(2.0f)), 0.0f}}

// #define FREI_CHEN_4 {{0.5f, -1.0f / (2.0f * sqrt(2.0f)), 0.0f},                       \
//                      {-1.0f / (2.0f * sqrt(2.0f)), 0.0f, 1.0f / (2.0f * sqrt(2.0f))}, \
//                      {0.0f, 1.0f / (2.0f * sqrt(2.0f)), -0.5f}}

// #define FREI_CHEN_5 {{0.0f, 1.0f / 2.0f, 0.0f},            \
//                      {-1.0f / 2.0f, 0.0f, -1.0f / 2.0f},   \
//                      {0.0f, 1.0f / 2.0f, 0.0f}}

// #define FREI_CHEN_6 {{-0.5f, 0.0f, 0.5f},    \
//                      {0.0f, 0.0f, 0.0f},                   \
//                      {0.5f, 0.0f, -0.5f}}

// #define FREI_CHEN_7 {{1.0f / 6.0f, -1.0f / 3.0f, 1.0f / 6.0f},    \
//                      {-1.0f / 3.0f, 2.0f / 3.0f, -1.0f / 3.0f},   \
//                      {1.0f / 6.0f, -1.0f / 3.0f, 1.0f / 6.0f}}

// #define FREI_CHEN_8 {{-1.0f / 3.0f, 1.0f / 6.0f, -1.0f / 3.0f},   \
//                      {1.0f / 6.0f, 2.0f / 3.0f, 1.0f / 6.0f},     \
//                      {-1.0f / 3.0f, 1.0f / 6.0f, -1.0f / 3.0f}}

// #define FREI_CHEN_9 {{1.0f / 3.0f, 1.0f / 3.0f, 1.0f / 3.0f},     \
//                      {1.0f / 3.0f, 1.0f / 3.0f, 1.0f / 3.0f},     \
//                      {1/0f / 3.0f, 1.0f / 3.0f, 1.0f / 3.0f}}

Image
Image::readJPEG(const char *fname)
{
    Image n_image;

    FILE *ifp;

    try { // catch any std::exception to throw custom one
        ifp = fopen(fname, "rb");
    } catch (...) {
        ifp = nullptr;
    }

    if (!ifp) {
        throw std::system_error(std::error_code(errno,
                                                std::generic_category()),
                                std::string("Could not open file ") + fname + " for reading");
    }

    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;
    cinfo.err = jpeg_std_error(&jerr);

    jpeg_create_decompress(&cinfo);
    jpeg_stdio_src(&cinfo, ifp);

    jpeg_read_header(&cinfo, TRUE);

    jpeg_start_decompress(&cinfo);

    n_image.w = cinfo.output_width;
    n_image.h = cinfo.output_height;
    switch (cinfo.out_color_space) {
        case JCS_CMYK:
            n_image.c_space = CMYK;
            n_image.image_data[CYAN] = std::vector<float>(n_image.width() *
                                                          n_image.height(), 0);
            n_image.image_data[MAGENTA] = std::vector<float>(n_image.width() *
                                                             n_image.height(), 0);
            n_image.image_data[YELLOW] = std::vector<float>(n_image.width() *
                                                            n_image.height(), 0);
            n_image.image_data[BLACK] = std::vector<float>(n_image.width() *
                                                           n_image.height(), 0);
            break;
        case JCS_EXT_RGBX:
        case JCS_EXT_BGRX:
        case JCS_EXT_XRGB:
        case JCS_EXT_XBGR:
            n_image.c_space = RGBX;
            n_image.image_data[RED] = std::vector<float>(n_image.width() *
                                                         n_image.height(), 0);
            n_image.image_data[BLUE] = std::vector<float>(n_image.width() *
                                                          n_image.height(), 0);
            n_image.image_data[GREEN] = std::vector<float>(n_image.width() *
                                                           n_image.height(), 0);
            n_image.image_data[ALPHA_IGNORED] = std::vector<float>(n_image.width() *
                                                                   n_image.height(), 0);
            break;
        case JCS_EXT_RGBA:
        case JCS_EXT_BGRA:
        case JCS_EXT_ARGB:
        case JCS_EXT_ABGR:
            n_image.c_space = RGBA;
            n_image.image_data[RED] = std::vector<float>(n_image.width() *
                                                         n_image.height(), 0);
            n_image.image_data[BLUE] = std::vector<float>(n_image.width() *
                                                          n_image.height(), 0);
            n_image.image_data[GREEN] = std::vector<float>(n_image.width() *
                                                           n_image.height(), 0);
            n_image.image_data[ALPHA] = std::vector<float>(n_image.width() *
                                                           n_image.height(), 0);
            break;
        case JCS_EXT_RGB:
        case JCS_EXT_BGR:
        case JCS_RGB:
        case JCS_RGB565:
            n_image.c_space = RGB;
            n_image.image_data[RED] = std::vector<float>(n_image.width() *
                                                         n_image.height(), 0);
            n_image.image_data[BLUE] = std::vector<float>(n_image.width() *
                                                          n_image.height(), 0);
            n_image.image_data[GREEN] = std::vector<float>(n_image.width() *
                                                           n_image.height(), 0);
            break;
        case JCS_YCbCr:
            n_image.c_space = YCbCr;
            n_image.image_data[INTENSITY] = std::vector<float>(n_image.width() *
                                                               n_image.height(), 0);
            n_image.image_data[Cb] = std::vector<float>(n_image.width() *
                                                        n_image.height(), 0);
            n_image.image_data[Cr] = std::vector<float>(n_image.width() *
                                                        n_image.height(), 0);
            break;
        case JCS_GRAYSCALE:
            n_image.c_space = GRAY;
            n_image.image_data[INTENSITY] = std::vector<float>(n_image.width() *
                                                               n_image.height(), 0);
            break;
        case JCS_YCCK:
        case JCS_UNKNOWN:
            throw std::logic_error("Unsupported JPEG color space");
    }

    // mapper between component number and ChannelType
    std::map<int, ChannelType> channelMapper;

    switch (cinfo.out_color_space) {
        case JCS_EXT_RGBX:
            channelMapper[0] = RED;
            channelMapper[1] = GREEN;
            channelMapper[2] = BLUE;
            channelMapper[3] = ALPHA_IGNORED;
            break;
        case JCS_EXT_RGBA:
            channelMapper[3] = ALPHA;
            // fall through
        case JCS_EXT_RGB:
        case JCS_RGB:
        case JCS_RGB565:
            channelMapper[0] = RED;
            channelMapper[1] = GREEN;
            channelMapper[2] = BLUE;
            break;
        case JCS_EXT_BGRX:
            channelMapper[2] = RED;
            channelMapper[1] = GREEN;
            channelMapper[0] = BLUE;
            channelMapper[3] = ALPHA_IGNORED;
            break;
        case JCS_EXT_BGRA:
            channelMapper[3] = ALPHA;
            // fall through
        case JCS_EXT_BGR:
            channelMapper[2] = RED;
            channelMapper[1] = GREEN;
            channelMapper[0] = BLUE;
           break;
        case JCS_EXT_XRGB:
            channelMapper[1] = RED;
            channelMapper[2] = GREEN;
            channelMapper[3] = BLUE;
            channelMapper[0] = ALPHA_IGNORED;
            break;
        case JCS_EXT_ARGB:
            channelMapper[1] = RED;
            channelMapper[2] = GREEN;
            channelMapper[3] = BLUE;
            channelMapper[0] = ALPHA;
            break;
        case JCS_EXT_XBGR:
            channelMapper[3] = RED;
            channelMapper[2] = GREEN;
            channelMapper[1] = BLUE;
            channelMapper[0] = ALPHA_IGNORED;
            break;
        case JCS_EXT_ABGR:
            channelMapper[3] = RED;
            channelMapper[2] = GREEN;
            channelMapper[1] = BLUE;
            channelMapper[0] = ALPHA;
            break;
        case JCS_CMYK:
            channelMapper[0] = CYAN;
            channelMapper[1] = MAGENTA;
            channelMapper[2] = YELLOW;
            channelMapper[3] = BLACK;
            break;
        case JCS_YCbCr:
            channelMapper[0] = INTENSITY;
            channelMapper[1] = Cb;
            channelMapper[2] = Cr;
            break;
        case JCS_GRAYSCALE:
            channelMapper[0] = INTENSITY;
            break;
        case JCS_YCCK:
        case JCS_UNKNOWN:
            throw std::logic_error("Unsupported JPEG color space");
    }

    // ith pixel belonging to channel comp will be stored @ cinfo.num_components * i + comp
    JSAMPLE *row_buffer = new JSAMPLE[cinfo.output_width * cinfo.num_components];
   
    while (cinfo.output_scanline < cinfo.output_height) {
        jpeg_read_scanlines(&cinfo, &row_buffer, 1);

        for (auto it = channelMapper.begin();
             it != channelMapper.end();
             ++it)
            for (ssize_t i = 0; i < n_image.width(); i++)
                n_image(it->second, i, cinfo.output_scanline - 1)
                    = row_buffer[cinfo.num_components * i + it->first];
    }

    jpeg_finish_decompress(&cinfo);

    delete[] row_buffer;
    jpeg_destroy_decompress(&cinfo);
    fclose(ifp);

    return n_image;
}

// Image
// Image::readPNG(const char *fname)
// {

// }

void
Image::writeJPEG(const char *fname,
                 const int quality) const
{
    FILE *ofp;

    struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr jerr;

    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_compress(&cinfo);

    try { // catch any std::exception to throw custom one.
        ofp = fopen(fname, "wb");
    } catch (...) {
        ofp = nullptr;
    }

    if (!ofp) {
        throw std::system_error(std::error_code(errno,
                                                std::generic_category()),
                                std::string("Could not open file ") + fname + " for reading");
    }
    jpeg_stdio_dest(&cinfo, ofp);

    cinfo.image_width = width();
    cinfo.image_height = height();
    cinfo.input_components = image_data.size();

    switch (colorSpace()) {
        case RGB:
            cinfo.in_color_space = JCS_RGB;
            break;
        case RGBX:
            cinfo.in_color_space = JCS_EXT_RGBX;
            break;
        case RGBA:
            cinfo.in_color_space = JCS_EXT_RGBA;
            break;
        case CMYK:
            cinfo.in_color_space = JCS_CMYK;
            break;
        case YCbCr:
            cinfo.in_color_space = JCS_YCbCr;
            break;
        case GRAY:
            cinfo.in_color_space = JCS_GRAYSCALE;
            break;
    }

    jpeg_set_defaults(&cinfo);
    jpeg_set_quality(&cinfo, quality, TRUE /* limit to baseline-JPEG values */);
    jpeg_start_compress(&cinfo, TRUE);

    // create a row buffer and merge all components
    JSAMPLE *row_buffer = new JSAMPLE[cinfo.image_width * cinfo.num_components];

    while (cinfo.next_scanline < cinfo.image_height) {
        for (ssize_t i = 0; i < width(); i++)
            switch (colorSpace()) {
                case RGB:
                    row_buffer[cinfo.num_components * i] = get(RED, i, cinfo.next_scanline);
                    row_buffer[cinfo.num_components * i + 1] = get(GREEN, i, cinfo.next_scanline);
                    row_buffer[cinfo.num_components * i + 2] = get(BLUE, i, cinfo.next_scanline);
                    break;
                case RGBX:
                    row_buffer[cinfo.num_components * i] = get(RED, i, cinfo.next_scanline);
                    row_buffer[cinfo.num_components * i + 1] = get(GREEN, i, cinfo.next_scanline);
                    row_buffer[cinfo.num_components * i + 2] = get(BLUE, i, cinfo.next_scanline);
                    row_buffer[cinfo.num_components * i + 3] = get(ALPHA_IGNORED, i, cinfo.next_scanline);
                   break;
                case RGBA:
                    row_buffer[cinfo.num_components * i] = get(RED, i, cinfo.next_scanline);
                    row_buffer[cinfo.num_components * i + 1] = get(GREEN, i, cinfo.next_scanline);
                    row_buffer[cinfo.num_components * i + 2] = get(BLUE, i, cinfo.next_scanline);
                    row_buffer[cinfo.num_components * i + 3] = get(ALPHA, i, cinfo.next_scanline);
                    break;
                case CMYK:
                    row_buffer[cinfo.num_components * i] = get(CYAN, i, cinfo.next_scanline);
                    row_buffer[cinfo.num_components * i + 1] = get(MAGENTA, i, cinfo.next_scanline);
                    row_buffer[cinfo.num_components * i + 2] = get(YELLOW, i, cinfo.next_scanline);
                    row_buffer[cinfo.num_components * i + 3] = get(BLACK, i, cinfo.next_scanline);
                   break;
                case YCbCr:
                    row_buffer[cinfo.num_components * i] = get(INTENSITY, i, cinfo.next_scanline);
                    row_buffer[cinfo.num_components * i + 1] = get(Cb, i, cinfo.next_scanline);
                    row_buffer[cinfo.num_components * i + 2] = get(Cr, i, cinfo.next_scanline);
                    break;
                case GRAY:
                    row_buffer[cinfo.num_components * i] = get(INTENSITY, i, cinfo.next_scanline);
                    break;
            } // extract data for different kinds of color spaces
        jpeg_write_scanlines(&cinfo, &row_buffer, 1);
    }

    jpeg_finish_compress(&cinfo);

    delete[] row_buffer;
    fclose(ofp);
    jpeg_destroy_compress(&cinfo);

}

std::string
Image::str() const
{
    std::stringstream os;
    os << "Image @ " << (const void *) (this) <<
        " { Width: " << std::to_string(width()) <<
        ", Height: " << std::to_string(height()) <<
        ", Color space: " << ::str(colorSpace()) << "}";
    return os.str();
}

std::string
Image::dump() const
{
    std::stringstream os;
    os << *this;
    return os.str();
}

std::ostream&
operator<<(std::ostream& os,
           const Image& im)
{
    os << "Color space: " << str(im.c_space) << std::endl;

    for (auto it = im.image_data.begin();
         it != im.image_data.end();
         ++it) {
        os << "Channel: " << str(it->first) << std::endl;
        for (ssize_t j = 0; j < im.height(); j++) {
            os << "[";
            for (ssize_t i = 0; i < im.width(); i++)
                os << im.get(it->first, i, j) << ", ";
            os << "]" << std::endl;
        }
        os << "---" << std::endl;
    }
    return os;
}

Image
operator+(const Image& im1,
          const Image& im2)
{
    if (im1.colorSpace() != im2.colorSpace())
        throw std::invalid_argument("Images must have the same colour space");
    if (im1.width() != im2.width())
        throw std::invalid_argument("Images must have the same width");
    if (im1.height() != im2.height())
        throw std::invalid_argument("Images must have the same height");

    Image n_im(im1);

    for (auto it = im1.image_data.begin(); it != im1.image_data.end(); ++it)
        for (ssize_t i = 0; i < im1.width(); i++)
            for (ssize_t j = 0; j < im1.height(); j++)
                n_im(it->first, i, j) += im2.get(it->first, i, j);

    return n_im;
}

Image
operator*(const Image& im1,
          const Image& im2)
{
    if (im1.colorSpace() != im2.colorSpace())
        throw std::invalid_argument("Images must have the same colour space");
    if (im1.width() != im2.width())
        throw std::invalid_argument("Images must have the same width");
    if (im1.height() != im2.height())
        throw std::invalid_argument("Images must have the same height");

    Image n_im(im1);

    for (auto it = im1.image_data.begin(); it != im1.image_data.end(); ++it)
        for (ssize_t i = 0; i < im1.width(); i++)
            for (ssize_t j = 0; j < im1.height(); j++)
                n_im(it->first, i, j) *= im2.get(it->first, i, j);

    return n_im;
}

Image
operator+(const Image& im,
          float x)
{
    Image n_im(im);

    for (auto it = im.image_data.begin(); it != im.image_data.end(); ++it)
        for (ssize_t i = 0; i < im.width(); i++)
            for (ssize_t j = 0; j < im.height(); j++)
                n_im(it->first, i, j) += x;

    return n_im;
}

Image
operator*(const Image& im,
          float x)
{
    Image n_im(im);

    for (auto it = im.image_data.begin(); it != im.image_data.end(); ++it)
        for (ssize_t i = 0; i < im.width(); i++)
            for (ssize_t j = 0; j < im.height(); j++)
                n_im(it->first, i, j) *= x;

    return n_im;
}

Image&
pow(Image& im,
    float p) {
    for (auto it = im.image_data.begin(); it != im.image_data.end(); ++it)
        for (ssize_t i = 0; i < im.width(); i++)
            for (ssize_t j = 0; j < im.height(); j++)
                im(it->first, i, j) = pow(im(it->first, i, j), p);

     return im;

}

Image&
sqrt(Image& im)
{
     for (auto it = im.image_data.begin(); it != im.image_data.end(); ++it)
        for (ssize_t i = 0; i < im.width(); i++)
            for (ssize_t j = 0; j < im.height(); j++)
                im(it->first, i, j) = sqrt(im(it->first, i, j));

     return im;
}

Image
atan2(const Image& im1,
      const Image& im2)
{
    if (im1.colorSpace() != im2.colorSpace())
        throw std::invalid_argument("Images must have the same colour space");
    if (im1.width() != im2.width())
        throw std::invalid_argument("Images must have the same width");
    if (im1.height() != im2.height())
        throw std::invalid_argument("Images must have the same height");

    Image n_im(im1.width(), im1.height(), im1.colorSpace());

    for (auto it = im1.image_data.begin(); it != im1.image_data.end(); ++it)
        for (ssize_t i = 0; i < im1.width(); i++)
            for (ssize_t j = 0; j < im1.height(); j++)
                n_im(it->first, i, j) = atan2(im1.get(it->first, i, j),
                                             im2.get(it->first, i, j));

    return n_im;
}
