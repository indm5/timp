cmake_minimum_required(VERSION 3.16)
project(Frontend)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets Network)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set(SOURCES
    main.cpp
    mainwindow.cpp
    authwidget.cpp
    registerwidget.cpp
    mainmenuwidget.cpp
    clientapi.cpp

)

set(HEADERS
    mainwindow.h
    authwidget.h
    registerwidget.h
    mainmenuwidget.h
    clientapi.h

)

set(FORMS
    mainwindow.ui
    authwidget.ui
    registerwidget.ui
    mainmenuwidget.ui
)

add_executable(Frontend ${SOURCES} ${HEADERS} ${FORMS})

target_link_libraries(Frontend
    Qt6::Core
    Qt6::Widgets
    Qt6::Network
)
