#include "elog.h"
#include <stdio.h>
#include <stdlib.h>
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
#define LOG_STANDARD_STREAM_OUT (stderr) 

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

#define CBUFFER_MAX_SIZE 4
int main(void)
{
    // circular buffer type shit.
    uint32_t head_idx = 0;
    uint32_t tail_idx = 0; 
    uint64_t count = 0;
    int cbuffer[CBUFFER_MAX_SIZE] = {0};
    
    for (int i = 0; i < 8; i++) {

        cbuffer[head_idx] = i;
        head_idx = (head_idx + 1) % CBUFFER_MAX_SIZE;
        if (head_idx == tail_idx) {
            tail_idx = (tail_idx + 1) % CBUFFER_MAX_SIZE;
        }
        if (count < CBUFFER_MAX_SIZE) {
            count++;
        }

    }

    int dummy_i = tail_idx;
    for (uint64_t i = 0; i < count; i++) {
        printf("%d ", cbuffer[dummy_i]);
        dummy_i = (dummy_i + 1) % CBUFFER_MAX_SIZE;
    }


#if 0 
    log_trace("");     
    log_debug("");
    log_info("");
    log_warning("");
    log_error("");
    log_fatal("");
#endif
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
   // entry.message = msg; // copy this somehow.
    entry.time = time(NULL); 
    entry.level = level;


    if (level >= g_state.min_accepted_level) {
        elog_s(log_level_to_string(level));
    }
}

void log_set_min_level(log_level level) 
{
    g_state.min_accepted_level = level;
}
