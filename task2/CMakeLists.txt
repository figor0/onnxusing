cmake_minimum_required(VERSION 3.5)

project(task2 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FindPkgConfig)

find_package(dlib REQUIRED)

add_executable(task2 main.cpp)

target_link_libraries(task2 PUBLIC dlib::dlib onnxruntime)
