cmake_minimum_required(VERSION 2.8)
project(Chromaticity)

find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -g -O3 ")

set(SOURCE_FILES main.cpp Sample.cpp Sample.h Histogram.h HSVSample.cpp HSVSample.h Polynomial.h YUVSample.cpp YUVSample.h Histogram2D.h YUVSample2.cpp YUVSample2.h GreenChromaClassifier.cpp)
add_executable(Chromaticity ${SOURCE_FILES})

target_link_libraries( Chromaticity ${OpenCV_LIBS} )
