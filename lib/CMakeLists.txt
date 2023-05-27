project(lib)
add_library(
        ${PROJECT_NAME}
        inc/app.h
        src/app.cpp
        src/task.cpp
        inc/task.h
        inc/fileblock.h
        src/fileblock.cpp
        inc/config.h
        src/config.cpp
)
add_library(lib::lib ALIAS ${PROJECT_NAME})


find_package(asio CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(jsoncpp CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)

target_link_libraries(lib PUBLIC asio::asio)
target_link_libraries(lib PUBLIC spdlog::spdlog)
target_link_libraries(lib PUBLIC JsonCpp::JsonCpp)
target_link_libraries(lib PUBLIC fmt::fmt)
target_include_directories(lib PUBLIC ./inc)