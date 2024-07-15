cmake_minimum_required(VERSION 3.22)
project(NES LANGUAGES C)
set(C_STANDARD C11)

add_custom_target(copy-font-png ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/font.png)
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/font.png
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/font.png
    ${CMAKE_CURRENT_BINARY_DIR}/font.png
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/font.png
)



add_executable(${PROJECT_NAME} main.c)



list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/sdl2)   # for sdl image

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)

include_directories(${PROJECT_NAME} ${SDL2_INCLUDE_DIRS})

target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES})
target_link_libraries(${PROJECT_NAME} ${SDL2_IMAGE_LIBRARIES})

