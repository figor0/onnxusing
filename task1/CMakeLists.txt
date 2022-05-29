cmake_minimum_required(VERSION 3.5)

project(task1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option( PROFILE "Profile an project" OFF)
if ( PROFILE )
    message(STATUS "Profiling is enabled")
    add_compile_options(-fsanitize=address)
    add_compile_options(-fsanitize=leak)
endif()
file(COPY resources DESTINATION ${PROJECT_BINARY_DIR})
set( CMAKE_CXX_FLAGS "-Wall -m64 -O3 " )

include(FindPkgConfig)

find_package(dlib REQUIRED)

add_executable(task1
    main.cpp
    Calculation.cpp
    Calculation.h
    RecognitionService.cpp
    RecognitionService.h
    helpers.h
    SystemDrawingService.h
    SystemDrawingService.cpp
)

if (PROFILE)
    target_link_libraries(task1 PUBLIC asan)
endif()

target_link_libraries(task1 PUBLIC dlib::dlib onnxruntime)
