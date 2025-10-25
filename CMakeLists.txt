cmake_minimum_required(VERSION 3.15)

project(Client LANGUAGES C)
add_executable(Client client.c)


project(Server LANGUAGES C)
add_executable(Server server.c)