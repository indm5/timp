cmake_minimum_required(VERSION 3.16)
project(Server)

find_package(Qt6 REQUIRED COMPONENTS Core Network Sql)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

set(SOURCES
    main.cpp
    server.cpp
    database.cpp
    functionsserver.cpp
)

set(HEADERS
    server.h
    database.h
    functionsserver.h
)

add_executable(Server ${SOURCES} ${HEADERS})

target_link_libraries(Server
    Qt6::Core
    Qt6::Network
    Qt6::Sql
)
