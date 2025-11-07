cmake_minimum_required(VERSION 3.15)

project(TCP_Server LANGUAGES C)

set(src 
  server.c
  client_manger/client_manger.c
)

add_executable(TCP_Server ${src})
target_link_libraries(TCP_Server Network)
