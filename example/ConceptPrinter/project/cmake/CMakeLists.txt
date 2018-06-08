# Copyright (c) 2018 Viktor Kireev
# Distributed under the MIT License

cmake_minimum_required(VERSION 3.6)

project(ConceptPrinter CXX)

add_executable(ConceptPrinter ${CMAKE_CURRENT_SOURCE_DIR}/../../src/main.cpp)
set_property(TARGET ConceptPrinter PROPERTY CXX_STANDARD 17)
target_link_libraries(ConceptPrinter Upl)
