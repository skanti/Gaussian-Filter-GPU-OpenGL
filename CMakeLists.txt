cmake_minimum_required(VERSION 3.5)
project(GaussianFilterGPU CXX)

set(CMAKE_CXX_COMPILER "/usr/local/bin/c++-6")

SET(OPTIMIZATION_FLAGS "-O3 -fopenmp -fomit-frame-pointer -march=native")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OPTIMIZATION_FLAGS} -std=c++14 -framework OpenGL")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/build")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/build")

set(SOURCE_FILES src/GaussianFilter.cpp src/GaussianFilter.h
        src/GLSLProgram.cpp src/GLSLProgram.h
        src/Shader.cpp src/Shader.h
        src/GaussianBlur.h
        src/shader/GaussianFS.glsl src/shader/GaussianVS.glsl)

# create library
add_library(GaussianFilterGPU ${SOURCE_FILES})

# create test
set(OPENCV_DIR /usr/local/Cellar/opencv3/3.1.0_3)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src ${OPENCV_DIR}/include)
link_directories(${OPENCV_DIR}/lib)
add_executable(GaussianFilterGPUTest test/GaussianFilterTest.cpp test/Timer.h ${SOURCE_FILES})
target_link_libraries(GaussianFilterGPUTest GaussianFilterGPU Glew glfw3 opencv_core opencv_imgcodecs opencv_imgproc opencv_highgui opencv_features2d)
