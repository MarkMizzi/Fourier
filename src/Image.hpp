#ifndef __IMAGE_H_
#define __IMAGE_H_

#include <vector>
#include <map>
#include <stdexcept>
#include <iostream>
#include <atomic>
#include <cstdlib>

typedef enum ChannelType {
RED,
GREEN,
BLUE,
ALPHA,
ALPHA_IGNORED,
CYAN,
MAGENTA,
YELLOW,
BLACK,
INTENSITY,
Cb,
Cr,
} ChannelType;

inline
std::string
str(ChannelType ch) {
    switch (ch) {
        case RED: return "RED";
        case GREEN: return "GREEN";
        case BLUE: return "BLUE";
        case ALPHA: return "ALPHA";
        case ALPHA_IGNORED: return "ALPHA_IGNORED";
        case CYAN: return "CYAN";
        case MAGENTA: return "MAGENTA";
        case YELLOW: return "YELLOW";
        case BLACK: return "BLACK";
        case INTENSITY: return "INTENSITY";
        case Cb: return "Cb";
        case Cr: return "Cr";
    }
}

typedef enum ColorSpace {
RGB,
RGBX,
RGBA,
CMYK,
YCbCr,
GRAY,
} ColorSpace;

inline
std::string
str(ColorSpace c_space) {
    switch (c_space) {
        case RGB: return "RGB";
        case RGBX: return "RGBX";
        case RGBA: return "RGBA";
        case CMYK: return "CMYK";
        case YCbCr: return "YCbCr";
        case GRAY: return "GRAY";
    }
}

class Image {
    // The data for one channel is held in a vector of floats,
    //       organized in such a way that the pixel (i, j) is in the position w * j + i
    // Each individual pixel takes on a value of 0-255.
    // For many operations which export pixels, the exported pixel value is converted to some unsigned integer type.
    std::map<ChannelType,
             std::vector<float>> image_data;
    ColorSpace c_space;
    ssize_t w, h;

    // It is the responsibility of factory methods to call this method with the proper parameters,
    //     for example, no checks are made to see if image having ColorSpace RGB has only a RED, GREEN and BLUE channel.
    Image(const std::map<ChannelType,
                         std::vector<float>>& _image_data,
          const ColorSpace _c_space,
          ssize_t _w,
          ssize_t _h) :
        image_data { _image_data },
        c_space { _c_space },
        w { _w },
        h { _h } {}

    // access a single pixel, such as the RED pixel at (100, 200) in an RGB image.
    float& operator()(ChannelType ch,
                      ssize_t i,
                      ssize_t j) { return image_data[ch][w * j + i]; }

    float get(ChannelType ch,
                  ssize_t i,
                  ssize_t j) const { return image_data.at(ch)[w * j + i]; }

    // return a copy of the indicated row. Avoid in the API as it is somewhat expensive.
    // avoids returning a reference due to possibility of changing a single row's width.
    inline std::vector<float> operator()(ChannelType ch,
                                         ssize_t row) const;

    // convolves a single component, purely used to reduce size of operator* definition.
    inline void convolve_component(ChannelType ch,
                                   const std::vector<std::vector<float>>& kern);

    public:
        // copy constructor
        Image(const Image& im) :
            image_data { im.image_data },
            c_space { im.c_space },
            w { im.w },
            h { im.h } {};

        Image& operator=(const Image& im) {
            image_data = im.image_data;
            c_space = im.c_space;
            w = im.w;
            h = im.h;
            return *this;
        }

        // empty image constructor
        Image(ssize_t _w,
              ssize_t _h,
              ColorSpace _c_space) :
            w { _w },
            h { _h },
            c_space { _c_space } {
                switch (_c_space) {
                    case RGBX:
                         image_data[RED] = std::vector<float>(_w * _h, 0);
                         image_data[GREEN] = std::vector<float>(_w * _h, 0);
                         image_data[BLUE] = std::vector<float>(_w * _h, 0);
                         image_data[ALPHA] = std::vector<float>(_w * _h, 0);
                         break;
                    case RGBA:
                        image_data[ALPHA] = std::vector<float>(_w * _h, 0);
                        // fall through
                    case RGB:
                         image_data[RED] = std::vector<float>(_w * _h, 0);
                         image_data[GREEN] = std::vector<float>(_w * _h, 0);
                         image_data[BLUE] = std::vector<float>(_w * _h, 0);
                         break;
                    case CMYK:
                        image_data[CYAN] = std::vector<float>(_w * _h, 0);
                        image_data[MAGENTA] = std::vector<float>(_w * _h, 0);
                        image_data[YELLOW] = std::vector<float>(_w * _h, 0);
                        image_data[BLACK] = std::vector<float>(_w * _h, 0);
                        break;
                    case YCbCr:
                        image_data[INTENSITY] = std::vector<float>(_w * _h, 0);
                        image_data[Cb] = std::vector<float>(_w * _h, 0);
                        image_data[Cr] = std::vector<float>(_w * _h, 0);
                        break;
                    case GRAY:
                        image_data[INTENSITY] = std::vector<float>(_w * _h, 0);
                        break;
                }
            }

        Image(){}

        ssize_t width() const { return w; }
        ssize_t height() const { return h; }
        ColorSpace colorSpace() const { return c_space; }

        void set_showing(bool _showing) { showing = _showing; }

        // Converts images to RGB.
        //  * RGB images untouched
        //  * RGBX and RGBA images have their alpha channels thrown away; fast
        void to_RGB();
        // Converts images to YCbCr.
        //  * YCbCr images untouched
        //  * gray images have two channels added; fast
        //  * RGBX and RGBA have their alpha channels thrown away and are then converted.
        void to_YCbCr();
        // Converts images to gray. Similar semantics to to_YCbCr().
        void to_gray();

        // convolve image
        Image& operator*(const std::vector<std::vector<float>>& kern);

        Image& gaussian_blur_naive(float std_dev,
                                   ssize_t kern_size_f);
        Image& gaussian_blur(float std_dev,
                             ssize_t kern_size_f);
        Image& canny_edge_detect(float blur_std_dev=1.4f,
                                 ssize_t blur_size_f=2,
                                 float upper_threshold=76.8,
                                 float lower_threshold=25.6);

        // writes the given JPEG to file with name fname.
        void writeJPEG(const char *fname, const int quality) const;
        // IMPLEMENT
        void writePNG(const char *fname) const;

        static Image readJPEG(const char *fname);
        // IMPLEMENT
        static Image readPNG(const char *fname);

        std::string str() const;
        std::string dump() const;

        friend std::ostream& operator<<(std::ostream& os,
                                        const Image& im);

        // elementwise arithmetic operations.
        // These may produce images with pixel values above 255.
        //       Such images cause undefined behaviour if written to a file.
        friend Image operator+(const Image& im1,
                               const Image& im2);
        friend Image operator*(const Image& im1,
                               const Image& im2);
        friend Image& pow(Image& im,
                          unsigned p);
        friend Image& sqrt(Image& im);
        friend Image atan2(const Image& im1,
                           const Image& im2);

        static float get_max_intensity() { return (float) ((1 << (sizeof(unsigned char) * 8)) - 1); }
};

inline
std::vector<float>
Image::operator()(ChannelType ch,
                  ssize_t row_i) const
{
    if (image_data.count(ch) > 0) {
        std::vector<float> row;
        for (ssize_t i = 0; i < w; i++)
            row.push_back(image_data.at(ch)[w * row_i + i]);
        return row;
    } else
        throw std::invalid_argument("Unsupported channel type.");
}

std::ostream&
operator<<(std::ostream& os,
           const Image& im);

Image
operator+(const Image& im1,
          const Image& im2);

Image
operator*(const Image& im1,
          const Image& im2);

Image&
pow(Image& im,
    unsigned p);

Image&
sqrt(Image& im);

Image
atan2(const Image& im1,
      const Image& im2);

#endif // __IMAGE_H_
