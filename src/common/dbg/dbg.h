/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @file dbg.h
 * @brief Provides debug and dump functions
 *
 * Provides debug functions with multiple levels and dump utilities.
 *
 */

#pragma once

#include <stdio.h>
#include <czmq.h>

typedef enum {
    DBG_LEVEL_INFO,
    DBG_LEVEL_DEBUG,
    DBG_LEVEL_WARNING,
    DBG_LEVEL_ERROR,
    DBG_LEVEL_SPECIAL,
} DbgLevel;

/** Get the name of the module object from the filename. __FILE__ differs between GCC on MinGW and Linux. */
#ifdef WIN32
#define __FILENAME__ (((strrchr(__FILE__, '\\')) != NULL) ? &(strrchr(__FILE__, '\\'))[1] : __FILE__)
#else
#define __FILENAME__ (((strrchr(__FILE__,  '/')) != NULL) ? &(strrchr(__FILE__,  '/'))[1] : __FILE__)
#endif

#define pause()                              \
    do {                                     \
        info("Press a key to continue...");  \
        getc (stdin);                        \
    } while (0);

/** Debug line template */
#define dbg_ex(level, output, format, ...)                            \
    do {                                                              \
        char *__time_now__ = zclock_timestr();                        \
        _dbg(level, "[%s][%s:%d in %s] " format,                      \
            __time_now__,                                             \
            __FILENAME__,                                             \
            __LINE__,                                                 \
            __FUNCTION__,                                             \
            ##__VA_ARGS__);                                           \
            zstr_free(&__time_now__);                                 \
    } while (0)

#define dbg_exnl(level, output, format, ...)                          \
    _dbg(level, format, ##__VA_ARGS__);

/** Buffer dump template */
#define buffer_print_ex(buffer, size, prefix)                         \
    do {                                                              \
        char *__time_now__ = zclock_timestr();                        \
        char *__prefix__ = zsys_sprintf(                              \
            "[%s][%s:%d in %s] %s",                                   \
            __time_now__,                                             \
            __FILENAME__,                                             \
            __LINE__,                                                 \
            __FUNCTION__,                                             \
            (prefix) ? prefix : "");                                  \
            _bufferPrint(buffer, size, __prefix__);                   \
            zstr_free(&__prefix__);                                   \
            zstr_free(&__time_now__);                                 \
    } while (0)


#if defined(__DBG_ACTIVATED__)

    // Declare debug functions here
    /** Basic level debug function. */
    #define dbg(format, ...)                                              \
        dbg_ex(DBG_LEVEL_DEBUG, stdout, format "\n", ##__VA_ARGS__)

    /** Warning level debug function. */
    #define warning(format, ...)                                          \
        dbg_ex(DBG_LEVEL_WARNING, stderr, "[WARNING] " format "\n", ##__VA_ARGS__)

    /** Error level debug function. */
    #define error(format, ...)                                            \
        dbg_ex(DBG_LEVEL_ERROR, stderr, "[ERROR] " format "\n", ##__VA_ARGS__)

    /** Special level debug function. */
    #define special(format, ...)                                            \
        dbg_ex(DBG_LEVEL_SPECIAL, stderr, "[SPECIAL] " format "\n", ##__VA_ARGS__)

    /** Fatal error level debug function. */
    #define die(format, ...)                                              \
        do {                                                              \
            dbg_ex(DBG_LEVEL_ERROR, stderr, "[FATAL ERROR] " format "\n", ##__VA_ARGS__); \
            pause(); \
            exit (-1);                                                    \
        } while (0)

    /** Dump a buffer into the standard output.
     * See _bufferPrint for the arguments documentation. */
    #define buffer_print(buffer, size, prefix) \
        buffer_print_ex (buffer, size, prefix)

#else
    // don't output anything if __DBG_ACTIVATED__ is not enabled
	#define dbg(format, ...)
	#define warning(format, ...)
	#define error(format, ...)
	#define die(format, ...)
    #define buffer_print(buffer, size, prefix) \
        buffer_print_ex (buffer, size, prefix)
	#define special(format, ...)
#endif

/** Info level debug function. Not a debug information */
#define info(format, ...)                                          \
    dbg_ex(DBG_LEVEL_INFO, stdout, format "\n", ##__VA_ARGS__)

// Colored output
#define info_white(format, ...)                                        \
    dbg_exnl(DBG_LEVEL_DEBUG, stdout, format, ##__VA_ARGS__)

#define info_green(format, ...)                                        \
    dbg_exnl(DBG_LEVEL_INFO, stdout, format, ##__VA_ARGS__)

#define info_yellow(format, ...)                                       \
    dbg_exnl(DBG_LEVEL_WARNING, stdout, format, ##__VA_ARGS__)

#define info_red(format, ...)                                          \
    dbg_exnl(DBG_LEVEL_ERROR, stdout, format, ##__VA_ARGS__)

#define info_blue(format, ...)                                         \
    dbg_exnl(DBG_LEVEL_SPECIAL, stdout, format, ##__VA_ARGS__)

/**
 * @brief Output a formated message to a chosen stream
 * @param level The debug level
 * @param output A destination stream
 * @param format the format of the message
 * @return
 */
void _dbg(int level, char *format, ...);


/**
 * @brief Dump a buffer in the standard output
 * @param buffer An allocated buffer to dump
 * @param bufferSize The buffer size
 * @param prefix A string printed before each line of the dump
 */
void _bufferPrint(void *buffer, int bufferSize, char *prefix);

/**
 * @brief Redirect the default output of the debug messages to a chosen FILE
 * @param output The destination stream of the debug messages
 */
void dbgSetOutput(FILE *output);
void dbgSetCustomOutput(char *filename);

/**
 * @brief Close the custom debug file
 */
void dbgClose(void);

// crash handlers
#ifdef WIN32
LONG WINAPI crashHandler(EXCEPTION_POINTERS *ExceptionInfo);
#else
#include <ucontext.h>
#include <execinfo.h>

void printTrace(void);
void crashHandler(int sig, siginfo_t *siginfo, void *_context);

#endif // WIN32
