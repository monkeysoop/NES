cmake_minimum_required(VERSION 3.22)
project(PPU LANGUAGES C)


add_library(${PROJECT_NAME} STATIC ppu.c)


add_dependencies(${PROJECT_NAME} PPU_BUS)


target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(${PROJECT_NAME} PUBLIC PPU_BUS)
target_link_libraries(${PROJECT_NAME} PRIVATE LOGGER)