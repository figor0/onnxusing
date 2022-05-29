cmake_minimum_required(VERSION 3.5)

project(task2 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option( PROFILE "Profile an project" OFF)
if ( PROFILE )
    message(STATUS "Profiling is enabled")
    add_compile_options(-fsanitize=address)
    add_compile_options(-fsanitize=leak)
endif()

include(FindPkgConfig)

find_package(dlib REQUIRED)

include_directories(include)

set(SOURCES
    src/SingleLineChineeseParser.cpp
    )

set(HEADERS
    include/neurolang/ITextParsingService.h
    include/neurolang/SingleLineChineeseParser.h
    include/neurolang/helpers.h
    )
file(COPY resources DESTINATION ${PROJECT_BINARY_DIR})
add_library(neurolang SHARED ${SOURCES} ${HEADERS})

add_executable(task2 main.cpp )

if (PROFILE)
    target_link_libraries(task2 asan)
    target_link_libraries(neurolang asan)
endif()


target_link_libraries(task2 neurolang dlib::dlib onnxruntime)
