cmake_minimum_required(VERSION 3.14)
project(GrayImageCompressor)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOURCES
    grayimagecompressor.cpp
)

add_library(GrayImageCompressor STATIC ${SOURCES})

target_include_directories(GrayImageCompressor PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})