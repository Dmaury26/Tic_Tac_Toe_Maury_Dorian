cmake_minimum_required(VERSION 3.10)
project(Tic_Tac_Toe_Maury_Dorian C)

set(CMAKE_C_STANDARD 11)

find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})
add_executable(Tic_Tac_Toe_Maury_Dorian main.c)
target_link_libraries(Tic_Tac_Toe_Maury_Dorian ${CURSES_LIBRARIES})

