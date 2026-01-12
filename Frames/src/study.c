#include <stdbool.h>
#if defined(SUPPORT_TRACELOG)
#define TRACELOG(level, ...) TraceLog(level, __VA_ARGS__)

#if defined(SUPPORT_TRACELOG_DEBUG)
    #define TRACELOGD(...) TraceLog(LOG_DEBUG, __VA_ARGS__)
#else
    #define TRACELOGD(...) (void)0
#endif
#else
#define TRACELOG(level, ...) (void)0
#define TRACELOGD(...) (void)0
#endif

// Callbacks to hook some internal functions
// WARNING: These callbacks are intended for advanced users
typedef void (*TraceLogCallback)(int logLevel, const char *text, va_list args);  // Logging: Redirect trace log messages
typedef unsigned char *(*LoadFileDataCallback)(const char *fileName, int *dataSize);    // FileIO: Load binary data
typedef bool (*SaveFileDataCallback)(const char *fileName, void *data, int dataSize);   // FileIO: Save binary data
typedef char *(*LoadFileTextCallback)(const char *fileName);            // FileIO: Load text data
typedef bool (*SaveFileTextCallback)(const char *fileName, const char *text); // FileIO: Save text data

static int logTypeLevel = LOG_INFO;                 // Minimum log type level
static TraceLogCallback traceLog = NULL;            // TraceLog callback function pointer
static LoadFileDataCallback loadFileData = NULL;    // LoadFileData callback function pointer
static SaveFileDataCallback saveFileData = NULL;    // SaveFileText callback function pointer
static LoadFileTextCallback loadFileText = NULL;    // LoadFileText callback function pointer
static SaveFileTextCallback saveFileText = NULL;    // SaveFileText callback function pointer
 

