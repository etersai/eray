// Public domain.
#ifndef ETER_DEBUG_H_
#define ETER_DEBUG_H_

// (*flashcard*)
//__FILE__, __LINE__ - self explain
//__DATE__ - compilation date 
//__TIME__ - compilation time
//__FUNCTION__ - current function name (compiler-specific) /*Good to know*/

#include <stdio.h>
#include <assert.h>

#ifdef ETER_DEBUG_LOG_ADD_NEW_LINE // this could be defined by the compiler.
#define DEBUG_LOG_NEW_LINE "\n"  
#else
#define DEBUG_LOG_NEW_LINE ""
#endif /*ETER_DEBUG_LOG_TOGGLE_NEW_LINE*/

#define STRINGIFY(x) #x // two step macro expansion.
#define TOSTRING(x) STRINGIFY(x)

#define DEBUG_LOG_OUT stderr
#define DEBUG_LOG_HEADER "[DEBUG] "
#define DEBUG_LOG_HEADER_PLUS "[DEBUG][" __FILE__ ":" TOSTRING(__LINE__) "] "

// LOGS
#define debug_log_float(f) do { fprintf(DEBUG_LOG_OUT, DEBUG_LOG_HEADER); fprintf(DEBUG_LOG_OUT, "[%f]\n", (f)); } while (0)
#define debug_log_int(i) do { fprintf(DEBUG_LOG_OUT, DEBUG_LOG_HEADER); fprintf(DEBUG_LOG_OUT, "[%d]\n", (i)); } while (0)

#define debug_log_token(var) do { fprintf(DEBUG_LOG_OUT, DEBUG_LOG_HEADER STRINGIFY(var) DEBUG_LOG_NEW_LINE); } while (0)
#define debug_log_simple(msg) do { fprintf(DEBUG_LOG_OUT, DEBUG_LOG_HEADER msg DEBUG_LOG_NEW_LINE); } while (0)
#define debug_log_simple_file_n_line(msg) do { fprintf(DEBUG_LOG_OUT, DEBUG_LOG_HEADER_PLUS msg DEBUG_LOG_NEW_LINE); } while (0)
#define debug_log_format(...) \
    do { \
        fprintf(DEBUG_LOG_OUT, DEBUG_LOG_HEADER); \
        fprintf(DEBUG_LOG_OUT, __VA_ARGS__); \
        fprintf(DEBUG_LOG_OUT, DEBUG_LOG_NEW_LINE); \
    } while (0) 
#define debug_log_format_file_n_line(...) \
    do { \
        fprintf(DEBUG_LOG_OUT, DEBUG_LOG_HEADER_PLUS); \
        fprintf(DEBUG_LOG_OUT, __VA_ARGS__); \
        fprintf(DEBUG_LOG_OUT, DEBUG_LOG_NEW_LINE); \
    } while (0)

// ASSERTS                                          //"" forces it to be string literal.
#define debug_assert(expr, msg) do {assert((expr) && ("" msg));} while (0)
#define debug_perma_assert(expr, msg) \
    do { \
        if (!(expr)) { \
            fprintf(DEBUG_LOG_OUT, DEBUG_LOG_HEADER_PLUS); \
            fprintf(DEBUG_LOG_OUT, "Assertion failed: %s : %s\n", #expr, msg); \
            abort(); \
        } \
    } while (0)

#endif /* ETER_DEBUG_H_ [etersai]*/
