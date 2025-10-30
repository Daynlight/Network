cmake_minimum_required(VERSION 3.15)

project(Network LANGUAGES C)

set(src 
  network.c
  client_manger/client_manger.c
  compression/compression.c
  encryption/encryption.c
)

add_library(Network ${src})
target_include_directories(Network PUBLIC ".")

if(WIN32)
  target_compile_definitions(Network PUBLIC "WIN32")
  target_link_libraries(Network ws2_32)
endif()