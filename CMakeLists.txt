cmake_minimum_required (VERSION 3.15)
project(ImageCompressor)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Qml Quick Concurrent)

add_subdirectory(GrayImageCompressor)

set(headers
    include/filemodel.h
    include/fileprocessor.h
)

set(sources
    src/main.cpp
    src/filemodel.cpp
    src/fileprocessor.cpp
)
set(resources qml.qrc)

qt_add_resources(sources ${resources})

add_executable(ImageCompressor ${headers} ${sources})

target_include_directories(ImageCompressor PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(ImageCompressor Qt::Core Qt::Gui Qt::Qml Qt::Quick Qt::Concurrent GrayImageCompressor)
