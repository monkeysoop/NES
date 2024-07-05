cmake_minimum_required(VERSION 3.18)
project(NES LANGUAGES C)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK4 REQUIRED IMPORTED_TARGET gtk4)

add_executable(NES main.c)

add_subdirectory(CPU)

target_link_libraries(NES PRIVATE PkgConfig::GTK4)