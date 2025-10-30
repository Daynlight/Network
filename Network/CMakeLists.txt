cmake_minimum_required(VERSION 3.15)

project(Network LANGUAGES C)

add_library(Network network.c clientManager.c)
target_include_directories(Network PUBLIC ".")

if(WIN32)
  target_compile_definitions(Network PUBLIC "WIN32")
  target_link_libraries(Network ws2_32)
endif()