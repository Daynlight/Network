cmake_minimum_required(VERSION 3.15)

project(Network LANGUAGES C)

add_library(Network network.c)
target_include_directories(Network PUBLIC ".")