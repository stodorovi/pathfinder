cmake_minimum_required(VERSION 3.20)

project(PathFinderApp
    VERSION 0.1.2
    LANGUAGES CXX
)

add_subdirectory(routing/tests)

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
    ui/components/toolbar/include/icon_button.h
    ui/components/toolbar/source/icon_button.cpp
    ui/components/toolbar/include/algorithm_selection.h
    ui/components/toolbar/source/algorithm_selection.cpp
    ui/components/toolbar/include/grid_creation_dialog.h
    ui/components/toolbar/source/grid_creation_dialog.cpp
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
    ui/components/toolbar/icons/create_new_grid_button.svg
)

foreach(icon_src ${icons_sources})
    file(COPY
        ${icon_src}
        DESTINATION ${CMAKE_BINARY_DIR}/resources/icons/
    )
endforeach()
