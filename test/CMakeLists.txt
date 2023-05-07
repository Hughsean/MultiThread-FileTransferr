project(TEST)

add_executable(T1 t1.cpp)
add_executable(T2 t2.cpp)
add_executable(T3 t3.cpp)
add_executable(T4 t4.cpp)
add_executable(T5 t5.cpp)

find_package(asio CONFIG REQUIRED)
find_package(jsoncpp CONFIG REQUIRED)
# find_package(gflags CONFIG REQUIRED)

target_link_libraries(T1 PRIVATE lib::lib)
target_link_libraries(T2 PRIVATE lib::lib)
target_link_libraries(T3 PRIVATE lib::lib)
target_link_libraries(T4 PRIVATE lib::lib)
target_link_libraries(T5 PRIVATE lib::lib)
