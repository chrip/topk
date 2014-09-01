project(topk)
cmake_minimum_required(VERSION 2.8)
aux_source_directory(. SRC_LIST)
SET(CMAKE_BUILD_TYPE Debug)
ADD_DEFINITIONS(-std=c++11)

add_executable(${PROJECT_NAME} ${SRC_LIST})

