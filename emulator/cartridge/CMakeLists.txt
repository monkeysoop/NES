cmake_minimum_required(VERSION 3.22)
project(CARTRIDGE LANGUAGES C)


add_subdirectory(mapper)


add_library(${PROJECT_NAME} STATIC cartridge.c)


add_dependencies(${PROJECT_NAME} LOGGER)


target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(${PROJECT_NAME} PRIVATE MAPPER)
target_link_libraries(${PROJECT_NAME} PRIVATE LOGGER)