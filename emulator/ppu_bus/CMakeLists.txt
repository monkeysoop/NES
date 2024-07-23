cmake_minimum_required(VERSION 3.22)
project(ppu_bus LANGUAGES C)
set(C_STANDARD C11)

add_library(${PROJECT_NAME} STATIC ppu_bus.c ppu_bus.h)