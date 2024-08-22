cmake_minimum_required(VERSION 3.22)
project(MAPPER LANGUAGES C)


add_library(${PROJECT_NAME} mapper_000.c mapper_001.c mapper_002.c mapper_003.c mapper_004.c)


add_dependencies(${PROJECT_NAME} CARTRIDGE)
add_dependencies(${PROJECT_NAME} LOGGER)


target_link_libraries(${PROJECT_NAME} PRIVATE CARTRIDGE)
target_link_libraries(${PROJECT_NAME} PRIVATE LOGGER)