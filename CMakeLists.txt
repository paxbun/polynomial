cmake_minimum_required(VERSION 3.13)
project(polynomial)
enable_testing()
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(polynomial-test polynomial-test.cc)
add_test(polynomial-test polynomial-test)