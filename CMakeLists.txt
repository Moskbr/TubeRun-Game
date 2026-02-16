cmake_minimum_required(VERSION 3.16)

project(TubeRun VERSION 0.1 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(SDL2 REQUIRED)
find_package(OpenGL REQUIRED)

add_executable(TubeRun
    src/main.cpp
    src/game.cpp
    src/renderer.cpp
    src/camera.cpp
    src/object3d.cpp
    src/cubemesh.cpp
    src/tubegrid.cpp
    src/quadmesh.cpp
)

target_include_directories(TubeRun PRIVATE src)


add_library(glad external/glad/src/glad.c)

target_include_directories(glad PUBLIC
    external/glad/include
)

target_link_libraries(TubeRun PRIVATE
    SDL2::SDL2
    SDL2::SDL2main
    glad
    OpenGL::GL
)

if (MSVC)
    target_compile_options(TubeRun PRIVATE /W4)
else()
    target_compile_options(TubeRun PRIVATE -Wall -Wextra -Wpedantic)
endif()

file(COPY assets DESTINATION ${CMAKE_BINARY_DIR})
