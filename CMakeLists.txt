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

set(grid_sources
    ui/components/grid/include/grid.h
    ui/components/grid/source/grid.cpp
)

set(ui_sources
    ui/components/main_frame/include/main_frame.h
    ui/components/main_frame/source/main_frame.cpp
    ${toolbar_sources}
    ${grid_sources}
)
    
set(sources
    source/pathfinder_app.cpp
    ${ui_sources}
)

add_executable(pathfinder
    ${sources}
)

target_include_directories(pathfinder
    PUBLIC ${PROJECT_SOURCE_DIR}
)

target_link_libraries(pathfinder
    PRIVATE
        Qt::Widgets
        Qt::Gui
)

set_property(TARGET pathfinder
    PROPERTY CXX_STANDARD 20
)

set(icons_sources
    ui/components/toolbar/icons/run_algorithm_button.svg
    ui/components/toolbar/icons/start_position_button.svg
    ui/components/toolbar/icons/end_position_button.svg
    ui/components/toolbar/icons/untraversable_node_button.svg
)

foreach(icon_src ${icons_sources})
    file(COPY
        ${icon_src}
        DESTINATION ${CMAKE_BINARY_DIR}/resources/icons/
    )
endforeach()
