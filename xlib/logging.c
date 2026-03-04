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

// Cbuffer
#define CB_TYPE(T, cap) typedef struct {T data[(cap)]; uint32_t head; uint32_t tail; uint32_t count;} CB##T;
#define cb_wipe(cb) (void) // TODO(eter): todo
#define cb_insert(cb, item) do { \
    (cb).data[(cb).head] = (item); \
    (cb).head = ((cb).head + 1) % (sizeof((cb).data)/sizeof((cb).data[0])); \
    if ((cb).head == (cb).tail) { \
        (cb).tail = ((cb).tail + 1) % (sizeof((cb).data)/sizeof((cb).data[0])); \
    } \
    if ((cb).count < (sizeof((cb).data)/sizeof((cb).data[0]))) { \
        (cb).count++; \
    } \
} while (0)

// Bascially this \/
// cbuffer[head] = i;
// head = (head + 1) % CBUFFER_MAX_SIZE;
// if (head == tail) {
//     tail = (tail + 1) % CBUFFER_MAX_SIZE;
// }
// if (count < CBUFFER_MAX_SIZE) {
//     count++;
// }

// typedef struct {
//     LogEntry entries[LOG_ENTRIES_MAX];
//     uint32_t head;
//     uint32_t tail; 
//     uint32_t count;
// } LogEntryCbuffer;

#define LOG_MESSAGE_MAX (256)
#define LOG_ENTRIES_MAX (128)
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

// expand circular buffer type.
CB_TYPE(LogEntry, LOG_ENTRIES_MAX)

typedef struct {
    CBLogEntry entries;
    log_level  min_accepted_level; // Zero initialized accepts all.
    bool       terminal_output;
} LogState;

internal LogState g_state;

void log_message(log_level level, const char* msg);
void log_set_min_level(log_level level);
internal LogEntry create_log_entry(log_level level, const char* msg);
internal const char* log_level_to_string(log_level level);

#define log_trace(msg) log_message(LEVEL_TRACE, (msg))
#define log_debug(msg) log_message(LEVEL_DEBUG, (msg))
#define log_info(msg) log_message(LEVEL_INFO, (msg))
#define log_warning(msg) log_message(LEVEL_WARNING, (msg))
#define log_error(msg) log_message(LEVEL_ERROR, (msg))
#define log_fatal(msg) log_message(LEVEL_FATAL, (msg))

int main(void)
{
    elog_u(g_state.entries.count);

    LogEntry xd = {0};
    cb_insert(g_state.entries, xd);


    elog_u(g_state.entries.count);


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
    if (level < g_state.min_accepted_level) {
        return;
    }

    LogEntry entry;
    strncpy()
   // entry.message = msg; // copy this somehow.
    entry.time = time(NULL); 
    entry.level = level;

}

internal LogEntry create_log_entry(log_level level, const char* msg)
{

}

void log_set_min_level(log_level level) 
{
    g_state.min_accepted_level = level;
}

// HEHE
//#define CBUFFER_MAX_SIZE 4
//// circular buffer type shit.
// uint32_t head = 0;
// uint32_t tail = 0; 
// uint64_t count = 0;
// int cbuffer[CBUFFER_MAX_SIZE] = {0};
//
// for (int i = 0; i < 8; i++) {
//
//     cbuffer[head] = i;
//     head = (head + 1) % CBUFFER_MAX_SIZE;
//     if (head == tail) {
//         tail = (tail + 1) % CBUFFER_MAX_SIZE;
//     }
//     if (count < CBUFFER_MAX_SIZE) {
//         count++;
//     }
//
// }
//
// int dummy_i = tail;
// for (uint64_t i = 0; i < count; i++) {
//     printf("%d ", cbuffer[dummy_i]);
//     dummy_i = (dummy_i + 1) % CBUFFER_MAX_SIZE;
// }
