cmake_minimum_required(VERSION 2.8)
project(Chromaticity)

find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++98 -g ")

set(SOURCE_FILES main.cpp Histogram2D.h Types.h Types.cpp GreenChromaClassifier.cpp)
add_executable(Chromaticity ${SOURCE_FILES})

target_link_libraries( Chromaticity ${OpenCV_LIBS} )
