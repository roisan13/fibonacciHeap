cmake_minimum_required(VERSION 3.27)
project(fibHeap)

set(CMAKE_CXX_STANDARD 23)

add_executable(fibHeap main.cpp
        FibonacciHeap.h
        FibonacciHeapMAX.h)
