cmake_minimum_required(VERSION 3.15)

project(Server LANGUAGES C)

set(src 
  server.c
  client_manger/client_manger.c
)

add_executable(Server ${src})
target_link_libraries(Server Network)
