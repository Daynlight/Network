cmake_minimum_required(VERSION 3.15)

project(UDP_Server LANGUAGES C)

set(src 
  server.c
)

add_executable(UDP_Server ${src})
target_link_libraries(UDP_Server Network)
