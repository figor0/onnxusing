cmake_minimum_required(VERSION 3.5)

project(task1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FindPkgConfig)

find_package(dlib REQUIRED)

#include(${PROJECT_SOURCE_DIR})

add_executable(task1
    main.cpp
    Calculation.cpp
    Calculation.h
    RecognitionService.cpp
    RecognitionService.h
    helpers.h
    SystemDrawingService.h
    SystemDrawingService.cpp)

target_link_libraries(task1 PUBLIC dlib::dlib onnxruntime)
