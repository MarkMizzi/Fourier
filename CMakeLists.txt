cmake_minimum_required(VERSION 3.10)
project(fourier)
set(CMAKE_CXX_STANDARD 11)
set(OpenGL_GL_PREFERENCE "GLVND")
set(CPPLIB_NAME fourier_cpp)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS "-shared -Wall -Wextra -fPIC")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

# Find python and pybind11 - both are required dependencies
find_package(Python2 COMPONENTS Interpreter Development)
find_package(pybind11 CONFIG)

# Without this, any build libraries automatically have names "lib{x}.so"
set(CMAKE_SHARED_MODULE_PREFIX "")

set(CPPLIB_SOURCE_FILES src/Image.cpp)
set(CPPLIB_HEADER_FILES src/Image.hpp
                        src/Kernel.hpp)

# Add the support library, this will be linked privately to all stuff exposed to python
add_library(${CPPLIB_NAME} STATIC ${CPPLIB_SOURCE_FILES} ${CPPLIB_HEADER_FILES})

# link libraries to C++ library
target_link_libraries(${CPPLIB_NAME} jpeg png)

# set up python module, link it to C++ library, and to Python and pybind11 libraries
pybind11_add_module(fourier src/fourier_PyModule.cpp)
target_link_libraries(fourier PRIVATE ${CPPLIB_NAME})
target_link_libraries(fourier PUBLIC ${PYTHON_LIBRARIES})
target_include_directories(fourier PRIVATE ${PYTHON_INCLUDE_DIRS} ${pybind11_INCLUDE_DIRS})

configure_file(src/__init__.py __init__.py COPYONLY)
