cmake_minimum_required(VERSION 3.22)
project(CPU LANGUAGES C)


add_library(${PROJECT_NAME} STATIC cpu.c)


add_dependencies(${PROJECT_NAME} CPU_BUS)
add_dependencies(${PROJECT_NAME} LOGGER)


target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(${PROJECT_NAME} PUBLIC CPU_BUS)
target_link_libraries(${PROJECT_NAME} PRIVATE LOGGER)