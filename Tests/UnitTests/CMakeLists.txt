cmake_minimum_required(VERSION 3.15)


project(UnitTests LANGUAGES C)

set(src 
  tests.c
)

set(headers
  compression_tests.h
)


add_executable(UnitTests ${src})

target_include_directories(UnitTests PUBLIC ".")