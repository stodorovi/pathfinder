cmake_minimum_required(VERSION 3.20)

project(PathFinderUI
    VERSION 0.1
    LANGUAGES CXX
)

find_package(Qt6 COMPONENTS Widgets Gui REQUIRED)
set(CMAKE_AUTOMOC ON)

add_executable(pathfinder
    ui/source/pathfinder_app.cpp
    ui/components/main_frame/source/main_frame.cpp
)

target_link_libraries(pathfinder
    PRIVATE Qt::Widgets
            Qt::Gui
)