// Public domain.
#ifndef ETER_DEBUG_H_
#define ETER_DEBUG_H_

// (*flashcard*)
//__FILE__, __LINE__ - self explain
//__DATE__ - compilation date 
//__TIME__ - compilation time
//__FUNCTION__ - current function name (compiler-specific) /*Good to know*/

#include <stdio.h>

#ifdef ETER_DEBUG_LOG_TOGGLE_NEW_LINE                
#define DEBUG_LOG_NEW_LINE "\n" // Cross platform shoudl be handled by stdio. 
#else
#define DEBUG_LOG_NEW_LINE ""
#endif /*ETER_DEBUG_LOG_TOGGLE_NEW_LINE*/

#define DEBUG_LOG_OUT stdout
#define DEBUG_LOG_HEADER "[DEBUG] "
#define DEBUG_LOG_HEADER_PLUS "[DEBUG][%s:%d]", __FILE__, __LINE__

#define debug_log_stringify(variable) do { fprintf(DEBUG_LOG_OUT, DEBUG_LOG_HEADER #variable DEBUG_LOG_NEW_LINE); } while (0)
#define debug_log_msg_simple(msg) do { fprintf(DEBUG_LOG_OUT, DEBUG_LOG_HEADER msg DEBUG_LOG_NEW_LINE); } while (0)
#define debug_log_append_file_n_line_simple(msg) do { fprintf(DEBUG_LOG_OUT, DEBUG_LOG_HEADER_PLUS msg DEBUG_LOG_NEW_LINE); } while (0)
#define debug_log_msg_format(...) \
    do { \
        fprintf(DEBUG_LOG_OUT, DEBUG_LOG_HEADER_PLUS); \
        fprintf(DEBUG_LOG_OUT, __VA_ARGS__); \
        fprintf(DEBUG_LOG_OUT, DEBUG_LOG_NEW_LINE); \
    } while (0) 
#define debug_log_append_file_n_line_format(...) \
    do { \
        fprintf(DEBUG_LOG_OUT, DEBUG_LOG_HEADER_PLUS); \
        fprintf(DEBUG_LOG_OUT, __VA_ARGS__); \
        fprintf(DEBUG_LOG_OUT, DEBUG_LOG_NEW_LINE); \
    } while (0)

#endif /* ETER_DEBUG_H_ [etersai]*/
