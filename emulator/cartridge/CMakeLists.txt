cmake_minimum_required(VERSION 3.22)
project(CARTRIDGE LANGUAGES C)
set(C_STANDARD C11)


add_subdirectory(mapper)


add_library(${PROJECT_NAME} STATIC cartridge.c)


target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})