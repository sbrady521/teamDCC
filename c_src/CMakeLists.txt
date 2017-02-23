cmake_minimum_required(VERSION 2.8)
project(Chromaticity)

find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -g -O3")

set(SOURCE_FILES main.cpp Histogram2D.h GreenChromaClassifier.cpp GreenChromaClassifier.h Types.h Types.cpp ColourCluster.cpp ColourCluster.h Colours.h)
add_executable(Chromaticity ${SOURCE_FILES})

target_link_libraries( Chromaticity ${OpenCV_LIBS} )
