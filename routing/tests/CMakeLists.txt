cmake_minimum_required(VERSION 3.20)

project(RoutingTests
    LANGUAGES CXX
)

add_executable(routingtests
    tests.cpp
)

set_property(TARGET routingtests
    PROPERTY CXX_STANDARD 20
)