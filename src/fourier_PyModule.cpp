#include "Image.hpp"

#include <pybind11/pybind11.h>
#include <pybind11/operators.h>
#include <pybind11/stl.h>

namespace py = pybind11;

PYBIND11_MODULE(fourier, m) {
    py::enum_<ColorSpace>(m, "ColorSpace")
        .value("RGB", ColorSpace::RGB)
        .value("RGBX", ColorSpace::RGBX)
        .value("RGBA", ColorSpace::RGBA)
        .value("CMYK", ColorSpace::CMYK)
        .value("YCbCr", ColorSpace::YCbCr)
        .value("Gray", ColorSpace::GRAY)
        .export_values();

    py::class_<Image>(m, "Image")
        .def(py::init<ssize_t, ssize_t, ColorSpace>())
        .def(py::init<const Image&>())
        .def("to_RGB", &Image::to_RGB)
        .def("to_YCbCr", &Image::to_YCbCr)
        .def("to_gray", &Image::to_gray)
        .def("__mul__", [](Image& im, std::vector<std::vector<float>> k){
             return im * k;
        }, py::is_operator())
        .def("__mul__", [](const Image& im1, const Image& im2){
             return im1 * im2;
        }, py::is_operator())
        .def("__add__", [](const Image& im1, const Image& im2){
             return im1 + im2;
        }, py::is_operator())
        .def("__mul__", [](const Image& im, float x){
             return im * x;
        }, py::is_operator())
        .def("__add__", [](const Image& im, float x){
             return im + x;
        }, py::is_operator())
        .def("__pow__", [](Image& im, float p){
             return pow(im, p);
        })
        .def("gaussian_blur_naive", &Image::gaussian_blur_naive,
             py::arg("std_dev"),
             py::arg("size_f"))
        .def("gaussian_blur", &Image::gaussian_blur,
             py::arg("std_dev"),
             py::arg("size_f"))
        .def("canny_edge_detect", &Image::canny_edge_detect,
             py::arg("blur_std_dev") = 1.4f,
             py::arg("blur_size_f") = 2,
             py::arg("upper_threshold") = 76.8,
             py::arg("lower_threshold") = 25.6)
        .def("writeJPEG", &Image::writeJPEG,
             py::arg("fname"),
             py::arg("quality") = 100)
        .def("__str__", &Image::str)
        .def("__repr__", &Image::str)
        .def("dump", &Image::dump);

    m.def("readJPEG",
          &Image::readJPEG,
          "A function which reads a JPEG into memory and wraps the pixel data in an Image object.",
          py::arg("fname"));
}
