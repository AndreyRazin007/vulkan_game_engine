#pragma once

#include "defines.h"

typedef struct platformState {
    void *internalState;
} platformState;

FAPI b8 platformStartup(
    platformState *platformState,
    const char *applicationName,
    i32 x,
    i32 y,
    i32 width,
    i32 height);

FAPI void platformShutdown(platformState *platformState);

FAPI b8 platformPumpMessages(platformState *platformState);

void *platformAllocate(u64 size, b8 aligned);
void platformFree(void *block, b8 aligned);
void *platformZeroMemory(void *block, u64 size);
void *platformCopyMemory(void *dest, const void *source, u64 size);
void *platformSetMemory(void *dest, i32 value, u64 size);

void platformConsoleWrite(const char *message, u8 colour);
void platformConsoleWriteError(const char *message, u8 colour);

f64 platformGetAbsoluteTime();

/* Sleep on the thread for the provided ms. This blocks the main thread.
 * Should only be used for giving time back to the OS for unused update power.
 * Therefore it is not exported.
 */
void platformSleep(u64 ms);
