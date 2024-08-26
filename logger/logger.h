#ifndef LOGGER_H
#define LOGGER_H

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>


#define EXIT_ON_WARNING 0


#define IGNORE_MESSAGE_CARTRIDGE  0
#define IGNORE_MESSAGE_MAPPER     0
#define IGNORE_MESSAGE_CONTROLLER 0
#define IGNORE_MESSAGE_CPU        0
#define IGNORE_MESSAGE_CPU_BUS    1
#define IGNORE_MESSAGE_PPU        0
#define IGNORE_MESSAGE_PPU_BUS    0
#define IGNORE_MESSAGE_EMULATOR   0
#define IGNORE_MESSAGE_MAIN       0

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
            if (IGNORE_MESSAGE_CARTRIDGE) { \
                if ((log_level == WARNING && EXIT_ON_WARNING) || log_level == ERROR) { \
                    exit(1); \
                } \
                continue; \
            } else { \
                printf("CARTRIDGE "); \
            } \
            break; \
        case MAPPER: \
            if (IGNORE_MESSAGE_MAPPER) { \
                if ((log_level == WARNING && EXIT_ON_WARNING) || log_level == ERROR) { \
                    exit(1); \
                } \
                continue; \
            } else { \
                printf("MAPPER "); \
            } \
            break; \
        case CONTROLLER: \
            if (IGNORE_MESSAGE_CONTROLLER) { \
                if ((log_level == WARNING && EXIT_ON_WARNING) || log_level == ERROR) { \
                    exit(1); \
                } \
                continue; \
            } else { \
                printf("CONTROLLER "); \
            } \
            break; \
        case CPU: \
            if (IGNORE_MESSAGE_CPU) { \
                if ((log_level == WARNING && EXIT_ON_WARNING) || log_level == ERROR) { \
                    exit(1); \
                } \
                continue; \
            } else { \
                printf("CPU "); \
            } \
            break; \
        case CPU_BUS: \
            if (IGNORE_MESSAGE_CPU_BUS) { \
                if ((log_level == WARNING && EXIT_ON_WARNING) || log_level == ERROR) { \
                    exit(1); \
                } \
                continue; \
            } else { \
                printf("CPU_BUS "); \
            } \
            break; \
        case PPU: \
            if (IGNORE_MESSAGE_PPU) { \
                if ((log_level == WARNING && EXIT_ON_WARNING) || log_level == ERROR) { \
                    exit(1); \
                } \
                continue; \
            } else { \
                printf("PPU "); \
            } \
            break; \
        case PPU_BUS: \
            if (IGNORE_MESSAGE_PPU_BUS) { \
                if ((log_level == WARNING && EXIT_ON_WARNING) || log_level == ERROR) { \
                    exit(1); \
                } \
                continue; \
            } else { \
                printf("PPU_BUS "); \
            } \
            break; \
        case EMULATOR: \
            if (IGNORE_MESSAGE_EMULATOR) { \
                if ((log_level == WARNING && EXIT_ON_WARNING) || log_level == ERROR) { \
                    exit(1); \
                } \
                continue; \
            } else { \
                printf("EMULATOR "); \
            } \
            break; \
        case MAIN: \
            if (IGNORE_MESSAGE_MAIN) { \
                if ((log_level == WARNING && EXIT_ON_WARNING) || log_level == ERROR) { \
                    exit(1); \
                } \
                continue; \
            } else { \
                printf("NES "); \
            } \
            break; \
    } \
\
    switch (log_level) { \
        case WARNING: \
            printf("WARNING:\n"); \
            break; \
        case INFO: \
            printf("INFO:\n"); \
            break; \
        case DEBUG_INFO: \
            printf("DEBUG INFO:\n"); \
            break; \
        case ERROR: \
            printf("ERROR:\n"); \
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