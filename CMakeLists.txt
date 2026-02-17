cmake_minimum_required(VERSION 3.14)

project(Sticky_Note
        DESCRIPTION "Sticky note app for linux Ubuntu made using C++ and QT"
        LANGUAGES CXX
)

file(READ "${CMAKE_SOURCE_DIR}/VERSION" APP_VERSION)
string(STRIP "${APP_VERSION}" APP_VERSION)

configure_file(
        "${CMAKE_SOURCE_DIR}/version.h.in"
        "${CMAKE_BINARY_DIR}/generated/version.h"
        @ONLY
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
        lib/main_window/main_window.cpp
        lib/note_action/note_action.cpp
        lib/note_window/note_window.cpp
        resources.qrc
)

target_link_libraries(sticky_note PRIVATE Qt6::Widgets)

target_include_directories(sticky_note PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/lib
        ${CMAKE_BINARY_DIR}/generated
)