cmake_minimum_required(VERSION 3.15)

project(UDPClient LANGUAGES C)


add_executable(UDPClient client.c)
target_link_libraries(UDPClient Network)