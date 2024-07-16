project(cartridge)

add_executable(${PROJECT_NAME} cartridge.c cartridge.h)

add_subdirectory(mapper)