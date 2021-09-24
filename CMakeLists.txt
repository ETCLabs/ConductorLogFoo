cmake_minimum_required(VERSION 3.15.3)

project(ConductorLogFoo VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOUIC_SEARCH_PATHS ui/)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 REQUIRED COMPONENTS Widgets Sql Core )

add_executable(ConductorLogFoo
    src/main.cpp
    src/mainwindow.cpp
    src/queryrunner.cpp
    src/calendarrangewidget.cpp
)


target_link_libraries(ConductorLogFoo Qt5::Widgets Qt5::Core Qt5::Sql)