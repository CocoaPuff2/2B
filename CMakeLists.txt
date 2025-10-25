cmake_minimum_required(VERSION 3.27)
project(2B)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(2B
        driver.cpp
        scheduler.cpp
        scheduler.h
        task.cpp)
