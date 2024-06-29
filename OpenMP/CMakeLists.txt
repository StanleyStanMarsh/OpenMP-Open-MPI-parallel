cmake_minimum_required(VERSION 3.24)
project(Fano_and_RLE_coding)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "-fopenmp")

add_executable(Fano_and_RLE_coding main.cpp "File Generation/FileGenerator.cpp" "File Generation/FileGenerator.h" Misc/alphabet.h Coding/FanoCoding.cpp Coding/FanoCoding.h Coding/RLECoding.cpp Coding/RLECoding.h Misc/misc.cpp Misc/misc.h)
