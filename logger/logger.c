#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "logger.h"


//void LOG(enum LogLevel log_level, const char* format, ...) {
//    switch (log_level) {
//        case WARNING: printf("Warning: "); break;
//        case INFO: printf("Info: "); break;
//        case DEBUG_INFO: printf("Debug Info: "); break;
//        case ERROR: printf("Error: "); break;
//    }
//    
//    va_list args;
//    va_start(args, format);
//    vprintf(format, args);
//    va_end(args);
//    printf("\n");
//
//    fflush(stdout);
//
//    if (log_level == ERROR) {
//        exit(1);
//    }
//        exit(1);
//}
//