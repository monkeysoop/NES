cmake_minimum_required(VERSION 3.22)
project(ppu LANGUAGES C)
set(C_STANDARD C11)

add_library(${PROJECT_NAME} STATIC ppu.c ppu.h)