cmake_minimum_required(VERSION 3.1)

project(visualization.glmviz LANGUAGES CXX)
set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_POSITION_INDEPENDENT_CODE 1)

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR})

set(CMAKE_CXX_STANDARD 14)

find_package(PkgConfig REQUIRED)
find_package(Kodi REQUIRED)
include_directories(${KODI_INCLUDE_DIR}/..)

find_package(OpenGL REQUIRED)
list(APPEND DEPLIBS ${OPENGL_LIBRARIES})
pkg_search_module(FFTW3 REQUIRED fftw3f libfftw3f)
list(APPEND DEPLIBS ${FFTW3_LIBRARIES})

set(ADDON_SOURCES src/addon.cpp src/Oscilloscope.cpp src/Program.cpp src/Triangle.cpp src/Buffer.cpp src/Spectrum2.cpp src/FFT.cpp)
set(ADDON_HEADERS src/Oscilloscope.hpp src/Program.hpp src/Triangle.hpp src/GL_utils.hpp src/Buffer.hpp src/Spectrum2.hpp src/FFT.hpp)

build_addon(visualization.glmviz ADDON DEPLIBS)

include(CPack)
