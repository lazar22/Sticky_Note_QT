cmake_minimum_required(VERSION 3.14)

project(Sticky_Note
        VERSION 0.0.1
        DESCRIPTION "Sticky note app for linux Ubuntu made using C++ and QT"
        LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

add_executable(sticky_note
        src/main.cpp
)

target_link_libraries(sticky_note PRIVATE Qt6::Widgets)