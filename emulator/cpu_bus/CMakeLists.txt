cmake_minimum_required(VERSION 3.22)
project(CPU_BUS LANGUAGES C)


add_library(${PROJECT_NAME} STATIC cpu_bus.c)


add_dependencies(${PROJECT_NAME} CARTRIDGE)
add_dependencies(${PROJECT_NAME} PPU)
add_dependencies(${PROJECT_NAME} CONTROLLER)
add_dependencies(${PROJECT_NAME} LOGGER)


target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(${PROJECT_NAME} PUBLIC CARTRIDGE)
target_link_libraries(${PROJECT_NAME} PUBLIC PPU)
target_link_libraries(${PROJECT_NAME} PUBLIC CONTROLLER)
target_link_libraries(${PROJECT_NAME} PRIVATE LOGGER)