cmake_minimum_required(VERSION 3.5)

project(FFmpegExtractor VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 COMPONENTS Widgets REQUIRED)

add_executable(FFmpegExtractor
    main.cpp
    mainwindow.cpp
    mainwindow.h
    aboutdialog.cpp
    aboutdialog.h
)

target_link_libraries(FFmpegExtractor PRIVATE Qt5::Widgets)
