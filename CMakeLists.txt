cmake_minimum_required(VERSION 3.7)
project(NES LANGUAGES C)


#add_custom_target(copy-main-ui ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/main.ui)
#add_custom_command(
#    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/main.ui
#    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/main.ui
#    ${CMAKE_CURRENT_BINARY_DIR}/main.ui
#    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/main.ui
#)

find_package(SDL2 REQUIRED)
include_directories(${PROJECT_NAME} ${SDL2_INCLUDE_DIRS})

add_executable(${PROJECT_NAME} main.c)
target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES})

