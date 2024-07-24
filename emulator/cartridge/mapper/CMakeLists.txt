cmake_minimum_required(VERSION 3.22)
project(MAPPER LANGUAGES C)


add_library(${PROJECT_NAME} mapper_000.c)


add_dependencies(${PROJECT_NAME} CARTRIDGE)


target_link_libraries(${PROJECT_NAME} PRIVATE CARTRIDGE)