#ifndef BEVY_LOG_H
#define BEVY_LOG_H

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define BEVY_LOG_VERSION "1.0"
typedef void (*log_LockFn)(void *data, int lock);

enum
{
    BEVY_LOG_TRACE,
    BEVY_LOG_DEBUG,
    BEVY_LOG_INFO,
    BEVY_LOG_WARN,
    BEVY_LOG_ERROR,
    BEVY_LOG_FATAL
};

#define bevy_log_trace(...) bevy_log(BEVY_LOG_TRACE, __FILE__, __LINE__, __VA_ARGS__)
#define bevy_log_debug(...) bevy_log(BEVY_LOG_DEBUG, __FILE__, __LINE__, __VA_ARGS__)
#define bevy_log_info(...) bevy_log(BEVY_LOG_INFO, __FILE__, __LINE__, __VA_ARGS__)
#define bevy_log_warn(...) bevy_log(BEVY_LOG_WARN, __FILE__, __LINE__, __VA_ARGS__)
#define bevy_log_error(...) bevy_log(BEVY_LOG_ERROR, __FILE__, __LINE__, __VA_ARGS__)
#define bevy_log_fatal(...) bevy_log(BEVY_LOG_FATAL, __FILE__, __LINE__, __VA_ARGS__)

static struct BevyLog
{
    void *data;
    log_LockFn lock_fn;
    FILE *file;
    int level;
    int quiet;
} BevyLog;

static const char *bevy_log_level_names[] = 
{
    "TRACE",
    "DEBUG",
    "INFO",
    "WARN",
    "ERROR",
    "FATAL"
};

static void bevy_log_lock(void)
{
    if(BevyLog.lock_fn)
    {
        BevyLog.lock_fn(BevyLog.data, 1);
    }
}

static void bevy_log_unlock(void)
{
    if(BevyLog.lock_fn)
    {
        BevyLog.lock_fn(BevyLog.data, 0);
    }
}

void bevy_log_set_data(void *data)
{
    BevyLog.data = data;
}

void bevy_log_set_lock_fn(log_LockFn fn)
{
    BevyLog.lock_fn = fn;
}

void bevy_log_set_file(FILE *file)
{
    BevyLog.file = file;
}

void bevy_log_set_level(int level)
{
    BevyLog.level = level;
}

void bevy_log_set_quiet(int enable)
{
    BevyLog.quiet = enable;
}

void bevy_log(int level, const char *file, int line, const char *fmt, ...)
{
    if(level < BevyLog.level)
    {
        return;
    }

    bevy_log_lock();

    time_t current_time = time(NULL);
    struct tm *local_time = localtime(&current_time);

    if(!BevyLog.quiet)
    {
        va_list args;
        char buffer[16];
        buffer[strftime(buffer, sizeof(buffer), "[%H:%M:%S]", local_time)];
        #ifdef BEVY_LOG_INCLUDE_FILE_PATH
            fprintf(stderr, "%s[%s][%s:%d]: ", buffer, bevy_log_level_names[level], file, line);
        #else
            fprintf(stderr, "%s[%s]: ", buffer, bevy_log_level_names[level]);
        #endif
        va_start(args, fmt);
        vfprintf(stderr, fmt, args);
        va_end(args);
        fprintf(stderr, "\n");
        fflush(stderr);
    }

    if(BevyLog.file)
    {
        va_list args;
        char buffer[32];
        buffer[strftime(buffer, sizeof(buffer), "[%H:%M:%S]", local_time)];
        fprintf(BevyLog.file, "%s[%s][%s:%d]: ", buffer, bevy_log_level_names[level], file, line);
        va_start(args, fmt);
        vfprintf(BevyLog.file, fmt, args);
        va_end(args);
        fprintf(BevyLog.file, "\n");
        fflush(BevyLog.file);
    }

    bevy_log_unlock();
}
#endif