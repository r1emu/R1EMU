/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @license GNU GENERAL PUBLIC LICENSE - Version 2, June 1991
 *          See LICENSE file for further information
 */

// ---------- Includes ------------
#ifndef WIN32
#define _GNU_SOURCE
#include <execinfo.h>
#endif
#include "dbg.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

// ------ Structure declaration -------
zmutex_t *mutex = NULL;
FILE *_output = NULL;

// ------ Static declaration -------

// ------ Extern function implementation ------

void dbgSetCustomOutput(char *filename) {

    // Special case : stdout
    if (strcmp(filename, "stdout") == 0) {
        dbgSetOutput(stdout);
        return;
    }

    FILE *out = fopen(filename, "a+");
    if (!out) {
        error("Cannot open filename '%s'", filename);
        return;
    }

    dbgSetOutput(out);
}

void dbgSetOutput(FILE *output) {
    _output = output;
}

void dbgClose(void) {
    if (_output && _output != stdout && _output != stderr) {
        fclose(_output);
    }

    _output = stdout;
}

void _dbg(int level, char *format, ...) {
    va_list args;

    if (mutex == NULL) {
        mutex = zmutex_new();
    }

    if (_output == NULL) {
        _output = stdout;
    }

    zmutex_lock(mutex);

    switch (level) {
        #ifdef WIN32
        case DBG_LEVEL_INFO:    SetConsoleTextAttribute(GetStdHandle (STD_OUTPUT_HANDLE), 0x0A); break;
        case DBG_LEVEL_DEBUG: break;
        case DBG_LEVEL_WARNING: SetConsoleTextAttribute(GetStdHandle (STD_OUTPUT_HANDLE), 0x0E); break;
        case DBG_LEVEL_ERROR:   SetConsoleTextAttribute(GetStdHandle (STD_OUTPUT_HANDLE), 0x0C); break;
        case DBG_LEVEL_SPECIAL: SetConsoleTextAttribute(GetStdHandle (STD_OUTPUT_HANDLE), 0x0B); break;
        #else
        case DBG_LEVEL_INFO: fprintf(_output, "\x1b[32m"); break;
        case DBG_LEVEL_DEBUG: fprintf(_output, "\x1b[37m"); break;
        case DBG_LEVEL_WARNING: fprintf(_output, "\x1b[33m"); break;
        case DBG_LEVEL_ERROR: fprintf(_output, "\x1b[31m"); break;
        case DBG_LEVEL_SPECIAL: fprintf(_output, "\x1b[35m"); break;
        #endif
    }

    va_start(args, format);
        vfprintf(_output, format, args);
    va_end(args);

    #ifdef WIN32
    SetConsoleTextAttribute(GetStdHandle (STD_OUTPUT_HANDLE), 0x07);
    #else
    fprintf(_output, "\033[0m");
    #endif

    fflush(_output);

    zmutex_unlock(mutex);
}

void
_bufferPrint(void *buffer, int bufferSize, char *prefix) {
    if (mutex == NULL) {
        mutex = zmutex_new();
    }

    zmutex_lock(mutex);

    int curPos = 0;

    fprintf(_output, "%s ===== buffer size = %d (0x%x) ================\n", prefix, bufferSize, bufferSize);

    while (curPos < bufferSize) {
        int offset;
        fprintf(_output, "%s", prefix);
        for (offset = 0; offset < 16 && curPos < bufferSize; offset++, curPos++) {
            fprintf(_output, " %02X", ((uint8_t *) buffer)[curPos]);
        }
        if (offset != 16) {
            for (int j = 0; j < 16 - offset; j++) {
                fprintf(_output, "   ");
            }
        }

        fprintf(_output, " | ");
        curPos -= offset;

        for (offset = 0; offset < 16 && curPos < bufferSize; offset++, curPos++) {
            uint8_t c = ((uint8_t *) buffer)[curPos];
            fprintf(_output, "%c", isprint(c) ? c : '.');
        }

        fprintf(_output, "\n");
    }
    fprintf(_output, "%s=================================================\n", prefix);
    fflush(_output);

    zmutex_unlock(mutex);
}


/* Crash handlers */
#ifdef WIN32
LONG WINAPI crashHandler(EXCEPTION_POINTERS *ExceptionInfo) {
    die("Application crashed at %p. Exception code = %x",
         ExceptionInfo->ContextRecord->Rip, ExceptionInfo->ExceptionRecord->ExceptionCode);

    return EXCEPTION_EXECUTE_HANDLER;
}

#else
void printTrace(void) {
    void *array[20];
    size_t size;
    char **strings;

    size = backtrace(array, sizeof(array) / sizeof(*(array)));
    strings = backtrace_symbols(array, size);

    error("Obtained %zd stack frames.", size);

    for (size_t i = 0; i < size; i++) {
        error("Frame %d : %s", i, strings[i]);
    }

    error("==================================");

    free(strings);
}

void crashHandler(int sig, siginfo_t *siginfo, void *_context) {
    int exceptionCode = siginfo->si_errno;
    ucontext_t *context = (ucontext_t*) _context;
    uintptr_t ip = context->uc_mcontext.gregs[REG_RIP];

    if (sig == SIGABRT) {
        printTrace();
    }

    die("Application crashed at %p. Exception code = %x", ip, exceptionCode);
}
#endif
