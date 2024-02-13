cmake_minimum_required(VERSION 3.20)

project(PathFinderUI
    VERSION 0.1.2
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
    ui/components/toolbar/icon_button/include/icon_button.h
    ui/components/toolbar/icon_button/source/icon_button.cpp
    ui/components/toolbar/algorithm_selection/include/algorithm_selection.h
    ui/components/toolbar/algorithm_selection/source/algorithm_selection.cpp
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

set_property(TARGET pathfinder
    PROPERTY CXX_STANDARD 20
)

file(COPY
    ui/components/toolbar/icons/run_algorithm_button.svg
    DESTINATION ${CMAKE_BINARY_DIR}/resources/icons/
)
file(COPY
    ui/components/toolbar/icons/start_position_button.svg
    DESTINATION ${CMAKE_BINARY_DIR}/resources/icons/
)