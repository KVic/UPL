# Copyright (c) 2018-2019 Viktor Kireev
# Distributed under the MIT License

cmake_minimum_required(VERSION 3.13...3.14.2)

project(Upl CXX)

set(UPL_INCLUDE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../include)
file(GLOB_RECURSE UPL_HEADERS ${UPL_INCLUDE_PATH}/*.h)

add_library(Upl)

target_compile_features(Upl PUBLIC cxx_std_17)
set_target_properties(Upl PROPERTIES LINKER_LANGUAGE CXX)

target_include_directories(Upl INTERFACE $<BUILD_INTERFACE: ${UPL_INCLUDE_PATH}>)

target_sources(Upl PRIVATE ${UPL_HEADERS})
