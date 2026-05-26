cmake_minimum_required(VERSION 3.16)
project(SRDMonitor VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_PREFIX_PATH "D:/Qt5/5.12.11/mingw73_64")

# 核心修改点：在 find_package 中追加 Sql 模块
find_package(Qt5 REQUIRED COMPONENTS Widgets Charts SerialPort Sql)

add_executable(SRDMonitor 
    src/main.cpp 
    src/MainWindow.h 
    src/MainWindow.cpp
)

# 核心修改点：在链接库中追加 Qt5::Sql
target_link_libraries(SRDMonitor PRIVATE Qt5::Widgets Qt5::Charts Qt5::SerialPort Qt5::Sql)