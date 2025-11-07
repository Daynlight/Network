cmake_minimum_required(VERSION 3.15)

project(TCP_Client LANGUAGES C)


add_executable(TCP_Client client.c)
target_link_libraries(TCP_Client Network)