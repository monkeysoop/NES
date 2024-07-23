cmake_minimum_required(VERSION 3.22)
project(PPU_BUS LANGUAGES C)
set(C_STANDARD C11)


add_library(${PROJECT_NAME} STATIC ppu_bus.c)


add_dependencies(${PROJECT_NAME} CARTRIDGE)


target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(${PROJECT_NAME} PUBLIC CARTRIDGE)