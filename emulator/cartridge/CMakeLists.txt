cmake_minimum_required(VERSION 3.22)
project(cartridge LANGUAGES C)
set(C_STANDARD C11)

add_library(${PROJECT_NAME} STATIC cartridge.c cartridge.h)
