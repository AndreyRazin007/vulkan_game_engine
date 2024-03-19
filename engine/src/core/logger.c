#include "logger.h"
#include "asserts.h"

#include <stdio.h>
#include <string.h>
#include <stdarg.h>

b8 initializeLogging() {
    return TRUE;
}

void shutdownLogging() {}

void logOutput(log_level level, const char *message, ...) {
    const char *level_strings[6] = {"[FATAL]: ", "[ERROR]: ", "[WARNING]: ", "[INFO]: ", "[DEBUG]: ", "[TRACE]: "};

    /* b8 is_error = level < 2;
     *
     * Technically imposes a 32k character limit on a single log entry, but...
     * DON'T DO THAT! */
    char messageInput[32000];
    memset(messageInput, 0, sizeof(messageInput));

    /* Format original message.
     * NOTE: Oddly enough, MS's headers override the GCC/Clang va_list type with a "typedef char* va_list" in some
     * cases, and as a result throws a strange error here. The workaround for now is to just use __builtin_va_list,
     * which is the type GCC/Clang's va_start expects. */
    __builtin_va_list argPtr;
    va_start(argPtr, message);
    vsnprintf(messageInput, 32000, message, argPtr);
    va_end(argPtr);

    char outMessage[32000];
    sprintf(outMessage, "%s%s\n", level_strings[level], messageInput);

    /* platform-specific output. */
    printf("%s", outMessage);
}

void reportAssertionFailure(const char *expression, const char *message,
                            const char *file, i32 line) {
    logOutput(LOG_LEVEL_FATAL,
              "Assertion Failure: %s, message: '%s', in file: %s, line: %d\n",
              expression, message, file, line);
}