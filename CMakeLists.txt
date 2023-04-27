cmake_minimum_required(VERSION 3.25)
project(CN.CD)

set(CMAKE_CXX_STANDARD 20)
# set(CMAKE_EXPORT_COMPILE_COMMANDS True)
#configure_file(
#        ${PROJECT_SOURCE_DIR}/config.h.in
#        ${PROJECT_BINARY_DIR}/config.h
#)
#
## add the binary tree to the search path for include files
## so that we will find demoConfig.h
#include_directories(${PROJECT_BINARY_DIR})

add_subdirectory(./log)
add_subdirectory(./test)
add_subdirectory(./lib)
add_subdirectory(./ui)