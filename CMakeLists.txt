cmake_minimum_required(VERSION 3.14)
project(Minesweeper)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp
)

add_executable(Minesweeper ${SOURCE_FILES})

find_package(SFML 2.5 COMPONENTS system window graphics network audio REQUIRED)
target_link_libraries(Minesweeper sfml-system sfml-window sfml-graphics sfml-audio sfml-network)