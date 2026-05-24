cmake_minimum_required(VERSION 3.22)
project(test_package CXX)

find_package(mbmff CONFIG REQUIRED)

add_executable(test_package example.cpp)
target_compile_features(test_package PRIVATE cxx_std_23)
target_link_libraries(test_package PRIVATE mbmff::mbmff)
