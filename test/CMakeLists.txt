project(TEST)

add_executable(app app.cpp)
add_executable(console console.cpp)

find_package(asio CONFIG REQUIRED)
find_package(jsoncpp CONFIG REQUIRED)

target_link_libraries(app PRIVATE lib::lib)
target_link_libraries(console PRIVATE lib::lib)


add_executable(t test.cpp)
target_link_libraries(t PRIVATE lib::lib)