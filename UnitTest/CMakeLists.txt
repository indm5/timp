cmake_minimum_required(VERSION 3.16)
project(TestGraphFunctions)

find_package(Qt6 REQUIRED COMPONENTS Core Test)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

set(FRONTSERV_PATH /Users/marina/Desktop/qwe/timp-Client_Kuzmin/Desktop/frontserv/server)

add_executable(TestGraphFunctions
    tst_testgraphfunctions.cpp
    ${FRONTSERV_PATH}/functionsserver.cpp
)

target_include_directories(TestGraphFunctions PRIVATE ${FRONTSERV_PATH})

target_link_libraries(TestGraphFunctions
    Qt6::Core
    Qt6::Test
)
