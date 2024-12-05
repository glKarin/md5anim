# cmake_minimum_required(VERSION <specify CMake version here>)
project(anim)

set(CMAKE_CXX_STANDARD 17)

include_directories(src)

set(CMAKE_BUILD_TYPE RelWithDebInfo)

add_executable(anim
        src/anim.cpp main.cpp src/text.cpp src/str.cpp src/geomath.cpp
        src/files.cpp src/mesh.cpp)
