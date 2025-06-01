#include "define.h"
#include <stdio.h>
#include <stdarg.h>
#include <time.h>

static const char *level_str[] = {
    "DEBUG", "INFO", "TRACE", "WARN", "ERROR", "FATAL"
};

void Log_message(LOG_LEVEL_e level, const char *file, const char *func, int line, const char *fmt, ...)
{
    char msg[256];
    va_list args;
    va_start(args, fmt);
    vsnprintf(msg, sizeof(msg), fmt, args);
    va_end(args);

    uint32_t now = (uint32_t)time(NULL);

    switch (level) {
        case LOG_DEBUG:
            printf("[%s] [%s->%s:%d] %s [time:%lu]\n", 
            level_str[level], file, func, line, msg, (unsigned long)now); 
            break;

        case LOG_INFO:
            printf("[%s] [%s] %s\n", level_str[level], func, msg); 
            break;

        case LOG_TRACE:
            printf("[%s] [%s]\n", level_str[level], func); 
            break;

        case LOG_WARN:
            printf("[%s] [%s] %s\n", level_str[level], func, msg); 
            break;

        case LOG_ERROR:
            printf("[%s] [%s->%s:%d] %s [time:%lu]\n", 
            level_str[level], file, func, line, msg, (unsigned long)now);
            break;

        case LOG_FATAL:
            printf("[%s] [%s->%s:%d] %s [time:%lu]\n", 
            level_str[level], file, func, line, msg, (unsigned long)now);
            while(1);
            break;

        default:
            printf("[UNKNOWN] %s\n", msg); 
            break;
    }
}