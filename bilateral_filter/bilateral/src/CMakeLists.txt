#Programa: CMakeLists.txt
#Curso: Herramientas de programación para procesamiento de señales 
#Módulo 7: http://iie.fing.edu.uy/rosaluna/wiki/Ipol:Modulo7#BilateralFilter
#Autor: Martin Etchart - mrtn.etchart@gmail.com

project(bilateral)

cmake_minimum_required(VERSION 2.6)

find_package(OpenCV)

set(CMAKE_C_FLAGS "-g -std=c99 ") #-pedantic-errors -Wall -Wextra -Werror")
set(CMAKE_L_FLAGS)

set(CMAKE_BUILD_TYPE "Release")
set(CMAKE_CXX_FLAGS "-D_ENGLISH")

set(bilateral_src bilateral.c iio.c color_spaces.c)
set(bilateral_inc bilateral.h iio.h color_spaces.h)
set(bilateral_libs m png jpeg tiff)

add_executable(test_bilateral_grayscale test_bilateral_grayscale.c ${bilateral_src})
target_link_libraries(test_bilateral_grayscale ${bilateral_libs})

add_executable(test_bilateral test_bilateral.c ${bilateral_src})
target_link_libraries(test_bilateral ${bilateral_libs})

if(OpenCV_FOUND)
    INCLUDE_DIRECTORIES(${OpenCV_INCLUDE_DIRS})
    LINK_DIRECTORIES(${OpenCV_LIB_DIR})

    add_executable(test_bilateral_opencv test_bilateral_opencv.c ${bilateral_src})
    target_link_libraries(test_bilateral_opencv ${bilateral_libs} ${OpenCV_LIBS})
endif(OpenCV_FOUND)
