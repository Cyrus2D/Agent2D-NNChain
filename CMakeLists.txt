cmake_minimum_required(VERSION 3.12)
project(nn1)

set(CMAKE_CXX_STANDARD 14)

add_executable(nn1 main.cpp src/dnn/Layer2d.h src/dnn/DNN2d.h)