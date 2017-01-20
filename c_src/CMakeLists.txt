cmake_minimum_required(VERSION 2.8)
project(Chromaticity)

find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -g ")

set(SOURCE_FILES main.cpp Sample.cpp Sample.h Histogram.h HSVSample.cpp HSVSample.h Polynomial.h YUVSample.cpp YUVSample.h)
add_executable(Chromaticity ${SOURCE_FILES})

target_link_libraries( Chromaticity ${OpenCV_LIBS} )