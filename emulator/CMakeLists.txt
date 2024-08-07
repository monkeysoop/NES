cmake_minimum_required(VERSION 3.22)
project(EMULATOR LANGUAGES C)


add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/cpu)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/cpu_bus)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/ppu)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/ppu_bus)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/cartridge)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/controller)


add_library(${PROJECT_NAME} STATIC emulator.c)


add_dependencies(${PROJECT_NAME} CPU)


target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(${PROJECT_NAME} PUBLIC CPU)