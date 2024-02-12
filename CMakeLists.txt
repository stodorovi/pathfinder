cmake_minimum_required(VERSION 3.20)

project(PathFinderUI
    VERSION 0.1.1
    LANGUAGES CXX
)

set(CMAKE_AUTOMOC ON)

find_package(Qt6
    COMPONENTS
        Widgets
        Gui
    REQUIRED
)

set(toolbar_sources
    ui/components/toolbar/include/toolbar.h
    ui/components/toolbar/source/toolbar.cpp

    ui/components/algorithm_selection/include/algorithm_selection.h
    ui/components/algorithm_selection/source/algorithm_selection.cpp
)

set(sources
    ui/source/pathfinder_app.cpp
    ui/components/main_frame/include/main_frame.h
    ui/components/main_frame/source/main_frame.cpp
    ${toolbar_sources}
)

add_executable(pathfinder
    ${sources}
)

target_link_libraries(pathfinder
    PRIVATE
        Qt::Widgets
        Qt::Gui
)