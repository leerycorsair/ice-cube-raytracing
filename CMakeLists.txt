cmake_minimum_required(VERSION 3.16)
project(Raytracing LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

find_package(OpenMP REQUIRED)

add_subdirectory(vendor)

set(SOURCES
    src/imgui.cpp
    src/geometry.h
    src/geometry.cpp
    src/renderer.h
    src/renderer.cpp
    src/objects.h
    src/objects.cpp
    src/model.h
    src/model.cpp
    src/application.h
    src/application.cpp
    src/main.cpp
    src/pch.h)

add_executable(${PROJECT_NAME} ${SOURCES})
target_precompile_headers(${PROJECT_NAME} PRIVATE src/pch.h)
target_link_libraries(${PROJECT_NAME} PRIVATE glfw glad imgui OpenMP::OpenMP_CXX)
