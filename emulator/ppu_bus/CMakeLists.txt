cmake_minimum_required(VERSION 3.22)
project(PPU_BUS LANGUAGES C)


add_library(${PROJECT_NAME} STATIC ppu_bus.c)


add_dependencies(${PROJECT_NAME} CARTRIDGE)
add_dependencies(${PROJECT_NAME} LOGGER)


target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(${PROJECT_NAME} PUBLIC CARTRIDGE)
target_link_libraries(${PROJECT_NAME} PRIVATE LOGGER)