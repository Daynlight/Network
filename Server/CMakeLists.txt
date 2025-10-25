cmake_minimum_required(VERSION 3.15)

project(Server LANGUAGES C)


add_executable(Server server.c)
target_link_libraries(Server Network)
