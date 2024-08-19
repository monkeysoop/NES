#ifndef LOGGER_H
#define LOGGER_H

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>


#define EXIT_ON_WARNING 0


#define IGNORE_CARTRIDGE  0
#define IGNORE_MAPPER     0
#define IGNORE_CONTROLLER 0
#define IGNORE_CPU        1
#define IGNORE_CPU_BUS    1
#define IGNORE_PPU        0
#define IGNORE_PPU_BUS    0
#define IGNORE_EMULATOR   0
#define IGNORE_MAIN       0

enum LogLevel {
    WARNING,
    INFO,
    DEBUG_INFO,
    ERROR,
};

enum LogSource {
    CARTRIDGE,
    MAPPER,
    CONTROLLER,
    CPU,
    CPU_BUS,
    PPU,
    PPU_BUS,
    EMULATOR,
    MAIN,
};

#define LOG(log_level, log_source, format, ...) do { \
    switch (log_source) { \
        case CARTRIDGE: \
            if (IGNORE_CARTRIDGE) { \
                continue; \
            } else { \
                printf("CARTRIDGE "); \
            } \
            break; \
        case MAPPER: \
            if (IGNORE_MAPPER) { \
                continue; \
            } else { \
                printf("MAPPER "); \
            } \
            break; \
        case CONTROLLER: \
            if (IGNORE_CONTROLLER) { \
                continue; \
            } else { \
                printf("CONTROLLER "); \
            } \
            break; \
        case CPU: \
            if (IGNORE_CPU) { \
                continue; \
            } else { \
                printf("CPU "); \
            } \
            break; \
        case CPU_BUS: \
            if (IGNORE_CPU_BUS) { \
                continue; \
            } else { \
                printf("CPU_BUS "); \
            } \
            break; \
        case PPU: \
            if (IGNORE_PPU) { \
                continue; \
            } else { \
                printf("PPU "); \
            } \
            break; \
        case PPU_BUS: \
            if (IGNORE_PPU_BUS) { \
                continue; \
            } else { \
                printf("PPU_BUS "); \
            } \
            break; \
        case EMULATOR: \
            if (IGNORE_EMULATOR) { \
                continue; \
            } else { \
                printf("EMULATOR "); \
            } \
            break; \
        case MAIN: \
            if (IGNORE_MAIN) { \
                continue; \
            } else { \
                printf("MAIN "); \
            } \
            break; \
    } \
\
    switch (log_level) { \
        case WARNING: \
            printf("WARNING\n"); \
            break; \
        case INFO: \
            printf("INFO\n"); \
            break; \
        case DEBUG_INFO: \
            printf("DEBUG INFO\n"); \
            break; \
        case ERROR: \
            printf("ERROR\n"); \
            break; \
    } \
\
    printf(format, ##__VA_ARGS__); \
    printf("\n"); \
    fflush(stdout); \
\
    if ((log_level == WARNING && EXIT_ON_WARNING) || log_level == ERROR) { \
        exit(1); \
    } \
} while (0)

#endif