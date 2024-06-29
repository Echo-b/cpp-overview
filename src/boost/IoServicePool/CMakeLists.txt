cmake_minimum_required(VERSION 3.15)
project(LogicServer)

set(CMAKE_CXX_STANDARD 14)
# aux_source_directory(${PROJECT_SOURCE_DIR} SRC)

# file(GLOB SRC ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

file(GLOB SRC ${PROJECT_SOURCE_DIR}/*.cpp)

# 排除不需要的文件
list(REMOVE_ITEM SRC ${PROJECT_SOURCE_DIR}/asyncClient.cpp)

find_package(jsoncpp REQUIRED)

add_executable(server ${SRC})

add_executable(client asyncClient.cpp)

target_link_libraries(server jsoncpp)
target_link_libraries(client jsoncpp)

