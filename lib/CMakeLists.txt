project(lib)
add_library(
        ${PROJECT_NAME}
        inc/connection.h
        src/connection.cpp
        src/worker.cpp
        inc/worker.h
        inc/fileblock.h src/fileblock.cpp inc/config.h)

target_include_directories(lib PUBLIC ./inc)
add_library(lib::lib ALIAS ${PROJECT_NAME})

#find_package(fmt CONFIG REQUIRED)
#target_link_libraries(log PRIVATE fmt::fmt)
find_package(asio CONFIG REQUIRED)
target_link_libraries(lib PRIVATE asio::asio)
target_link_libraries(lib PRIVATE lib::log)
