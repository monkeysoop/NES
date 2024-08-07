cmake_minimum_required(VERSION 3.22)
project(CONTROLLER LANGUAGES C)


add_library(${PROJECT_NAME} STATIC controller.c)


target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})