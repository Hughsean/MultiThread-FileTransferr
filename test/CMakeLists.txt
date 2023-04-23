project(TEST)

set(CMAKE_CXX_STANDARD 20)

add_executable(T1 test1.cpp)
add_executable(T2 test2.cpp)
add_executable(T3 test3.cpp)

find_package(asio CONFIG REQUIRED)
target_link_libraries(T1 PRIVATE asio::asio)
target_link_libraries(T2 PRIVATE asio::asio)
target_link_libraries(T3 PRIVATE asio::asio)

find_package(fmt CONFIG REQUIRED)
target_link_libraries(T1 PRIVATE fmt::fmt lib::log)
target_link_libraries(T2 PRIVATE fmt::fmt)
