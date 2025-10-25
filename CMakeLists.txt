cmake_minimum_required(VERSION 3.15)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)



add_subdirectory(Network)
add_subdirectory(Example/Chat/Server)
add_subdirectory(Example/Chat/Client)




add_custom_target(RUN_ALL
  COMMAND bash ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/../.github/run_both.sh
  DEPENDS Server Client
)
