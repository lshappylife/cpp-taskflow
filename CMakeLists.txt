cmake_minimum_required (VERSION 3.9)
MESSAGE(STATUS "CMAKE_ROOT: " ${CMAKE_ROOT})

# Project name
project (TaskFlow)

# Turn on the verbose
set(CMAKE_VERBOSE_MAKEFILE ON)

# CXX target properties
set(CMAKE_C_COMPILER "gcc")
set(CMAKE_CXX_COMPILER "g++")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
message(STATUS "Build type: " ${CMAKE_BUILD_TYPE})
message(STATUS "CMAKE_CXX_COMPILER: " ${CMAKE_CXX_COMPILER})
message(STATUS "CMAKE_CXX_FLAGS: " ${CMAKE_CXX_FLAGS})


# The version number
set (TASKFLOW_MAJOR_VERSION "0")
set (TASKFLOW_MINOR_VERSION "1")


# add the binayr tree to the search path for include files so we can find TutorialConfig.h
include_directories("${PROJECT_SOURCE_DIR}")
include_directories(doctest)


# Find pthread package
set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads REQUIRED)

# Find OpenMP package
find_package(OpenMP REQUIRED)

# -----------------------------------------------------------------------------
# Example program 
# -----------------------------------------------------------------------------
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/example)

add_executable(simple example/simple.cpp)
target_link_libraries(simple Threads::Threads)

add_executable(matrix example/matrix.cpp)
target_compile_options(matrix PRIVATE ${OpenMP_CXX_FLAGS})
target_link_libraries(matrix ${OpenMP_CXX_FLAGS} Threads::Threads)


# -----------------------------------------------------------------------------
# Unittest
# -----------------------------------------------------------------------------
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/unittest)
add_executable(taskflow unittest/taskflow.cpp)
target_link_libraries(taskflow Threads::Threads)
