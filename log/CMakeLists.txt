project(log)
add_library(log ./log.cpp inc/log.h)
add_library(lib::log ALIAS log)

target_include_directories(log PUBLIC inc)