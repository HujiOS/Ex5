cmake_minimum_required(VERSION 2.6)
project(Ex5)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(SOURCE_FILES whatsappServer.cpp Constants.h)
add_executable(Ex5 ${SOURCE_FILES})



project(Ex5Client)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(SOURCE_FILES whatsappClient.cpp Constants.h)
add_executable(Ex5Client ${SOURCE_FILES})