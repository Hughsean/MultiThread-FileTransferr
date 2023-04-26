project(TEST)

set(CMAKE_CXX_STANDARD 20)

add_executable(T1 test1.cpp)
add_executable(T2 test2.cpp)
add_executable(T3 test3.cpp)
add_executable(T4 test4.cpp)

find_package(asio CONFIG REQUIRED)
target_link_libraries(T1 PRIVATE asio::asio)
target_link_libraries(T2 PRIVATE asio::asio)
target_link_libraries(T3 PRIVATE asio::asio)

# set(THREADS_PREFER_PTHREAD_FLAG ON)
# set(THREADS_PREFER_PTHREAD_FLAG ON)
# option(CMAKE_USE_WIN32_THREADS_INIT "using WIN32 threads" ON)

find_package(fmt CONFIG REQUIRED)
find_package(jsoncpp CONFIG REQUIRED)
find_package(PThreads4W REQUIRED)


target_link_libraries(T1 PRIVATE fmt::fmt lib::log lib::lib JsonCpp::JsonCpp)
target_link_libraries(T2 PRIVATE fmt::fmt JsonCpp::JsonCpp)
target_link_libraries(T3 PRIVATE fmt::fmt JsonCpp::JsonCpp)
# target_link_libraries(T4 PRIVATE Threads::Threads)
target_link_libraries(T4 PRIVATE PThreads4W::PThreads4W)
