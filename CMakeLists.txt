cmake_minimum_required(VERSION 2.6)
project(Ex5)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread")
set(SOURCE_FILES WhatsappServer.cpp)
add_executable(Ex5 ${SOURCE_FILES})



project(Ex5Client)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread")
set(SOURCE_FILES WhatsappClient.cpp)
add_executable(Ex5Client ${SOURCE_FILES})