cmake_minimum_required (VERSION 3.13)

add_library(platform SHARED platform.c)
target_include_directories(platform PUBLIC ${CMAKE_SOURCE_DIR}/lib)
target_compile_options(platform PRIVATE -Wall)