project(log)
add_library(log ./log.cpp inc/log.h)
add_library(lib::log ALIAS log)

find_package(fmt CONFIG REQUIRED)
target_link_libraries(log PRIVATE fmt::fmt)

target_include_directories(log PUBLIC inc)