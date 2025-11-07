cmake_minimum_required(VERSION 3.15)

project(UDPServer LANGUAGES C)

set(src 
  server.c
)

add_executable(UDPServer ${src})
target_link_libraries(UDPServer Network)
