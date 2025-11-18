cmake_minimum_required(VERSION 3.15)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)



add_subdirectory(Network)

add_subdirectory(Examples/Chat/Server)
add_subdirectory(Examples/Chat/Client)

add_subdirectory(Examples/UDP/Server)
add_subdirectory(Examples/UDP/Client)

add_subdirectory(Examples/TCP/Server)
add_subdirectory(Examples/TCP/Client)

add_subdirectory(Examples/Web/Server)