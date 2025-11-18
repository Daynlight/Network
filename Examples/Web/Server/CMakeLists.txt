cmake_minimum_required(VERSION 3.15)

project(WebServer LANGUAGES C)

set(src 
  server.c
  client_manger/client_manger.c
)

add_executable(WebServer ${src})
target_link_libraries(WebServer Network)
