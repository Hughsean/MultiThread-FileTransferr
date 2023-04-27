project(MYGUI LANGUAGES CXX)
#cmake_minimum_required(VERSION 3.25)
# set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# set(CMAKE_CXX_STANDARD 17)
# set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core REQUIRED)
find_package(Qt6 REQUIRED COMPONENTS Gui REQUIRED)
find_package(Qt6 REQUIRED COMPONENTS Widgets REQUIRED)

add_executable(${PROJECT_NAME} main.cpp window.cpp window.h window.ui)

target_link_libraries(${PROJECT_NAME} Qt6::Core Qt6::Gui Qt6::Widgets)