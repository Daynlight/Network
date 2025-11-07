cmake_minimum_required(VERSION 3.15)

project(UDP_Client LANGUAGES C)


add_executable(UDP_Client client.c)
target_link_libraries(UDP_Client Network)