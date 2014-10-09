project(topk)
cmake_minimum_required(VERSION 2.8)


set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall")
set(CMAKE_BUILD_TYPE "Debug")
#set(CMAKE_BUILD_TYPE "Release")
add_definitions(-std=c++11)
add_subdirectory(src)

################################
# GTest
################################
#if(${CMAKE_BUILD_TYPE} MATCHES "Debug")
    enable_testing()
    add_subdirectory(gtest-1.7.0)

    include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
    add_subdirectory(test)
#endif()

