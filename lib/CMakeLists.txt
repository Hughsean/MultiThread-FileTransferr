project(lib)
add_library(
        ${PROJECT_NAME}
        inc/connection.h
        src/connection.cpp
        src/task.cpp
        inc/task.h
        inc/fileblock.h 
        src/fileblock.cpp 
        inc/config.h
)
add_library(lib::lib ALIAS ${PROJECT_NAME})


find_package(asio CONFIG REQUIRED)

target_link_libraries(lib PRIVATE asio::asio)
target_link_libraries(lib PRIVATE lib::log)

target_include_directories(lib PUBLIC ./inc)