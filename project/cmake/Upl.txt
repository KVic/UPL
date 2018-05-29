# Copyright (c) 2018 Viktor Kireev
# Distributed under the MIT License

cmake_minimum_required(VERSION 3.6)

project(Upl CXX)

set(UPL_INCLUDE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../include)
file(GLOB_RECURSE UPL_HEADERS ${UPL_INCLUDE_PATH}/*.h)

add_library(Upl INTERFACE)
target_include_directories(Upl INTERFACE $<BUILD_INTERFACE: ${UPL_INCLUDE_PATH}>)
target_sources(Upl INTERFACE ${UPL_HEADERS})
