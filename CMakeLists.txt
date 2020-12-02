project (ufdblock)

cmake_minimum_required (VERSION 3.9)

add_executable(ndf ndf.cpp)

target_compile_features(ndf PUBLIC cxx_std_17)
