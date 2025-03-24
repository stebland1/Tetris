cmake_minimum_required(VERSION 3.10)

project(Tetris C)

include_directories(${CMAKE_SOURCE_DIR}/include)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(tetris src/main.c src/game.c src/tetromino.c src/utils.c)
