#include "elog.h"
#include <stdio.h>
#include <time.h>
#include <stdint.h>
#include <stdbool.h>

// [16:46:12][INFO]: Engine initialized succesfuly.
// [16:46:12][WARNING]: Memory low.

// BASE
#define internal      static
#define global        static
#define local_persist static

#define unused(var) ((void)(var))

// ENDBASE

#define LOG_MESSAGE_MAX (256)
#define LOG_ENTRIES_MAX (64)

typedef enum {
    LEVEL_TRACE = 0,
    LEVEL_DEBUG,
    LEVEL_INFO,
    LEVEL_WARNING,
    LEVEL_ERROR,
    LEVEL_FATAL
} log_level;

typedef struct {
    char      message[LOG_MESSAGE_MAX];
    time_t    time;
    log_level level;
} LogEntry;

typedef struct {
    LogEntry  entries[LOG_ENTRIES_MAX];
    log_level min_accepted_level; // Zero initialized accepts all.
    bool      terminal_output;
} LogState;

internal LogState g_state;

void log_message(log_level level, const char* msg);
void log_set_min_level(log_level level);
internal const char* log_level_to_string(log_level level);

#define log_trace(msg) log_message(LEVEL_TRACE, (msg))
#define log_debug(msg) log_message(LEVEL_DEBUG, (msg))
#define log_info(msg) log_message(LEVEL_INFO, (msg))
#define log_warning(msg) log_message(LEVEL_WARNING, (msg))
#define log_error(msg) log_message(LEVEL_ERROR, (msg))
#define log_fatal(msg) log_message(LEVEL_FATAL, (msg))

int main(void)
{
    // circular buffer type shit.
    const size_t cbuffer_max_size = 256;
    
    
    uint32_t head = 0;
    uint32_t tail = 1;
    int cbuffer[cbuffer_max_size];



    log_trace("");     
    log_debug("");
    log_info("");
    log_warning("");
    log_error("");
    log_fatal("");
    return 0;
}

const char* log_level_to_string(log_level level)
{
    switch (level)
    {
        case LEVEL_TRACE:   return "[TRACE]";
        case LEVEL_DEBUG:   return "[DEBUG]";
        case LEVEL_INFO:    return "[INFO]";
        case LEVEL_WARNING: return "[WARNING]";
        case LEVEL_ERROR:   return "[ERROR]";
        case LEVEL_FATAL:   return "[FATAL]";
        default:            return "[???]";
    }
}

void log_message(log_level level, const char* msg)
{
    unused(msg);

    LogEntry entry; 
    entry.level = level;
    entry.message = msg; // copy this somehow.
    entry.time = time(NULL); 


    if (level >= g_state.min_accepted_level) {
        elog_s(log_level_to_string(level));
    }
}

void log_set_min_level(log_level level) 
{
    g_state.min_accepted_level = level;
}
