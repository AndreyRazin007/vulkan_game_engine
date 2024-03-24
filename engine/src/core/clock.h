#pragma once

#include "defines.h"

typedef struct clock {
    f64 startTime;
    f64 elapsed;
} clock;

/* Updates the provided clock. Should be called just before checking elapsed time.
 * Has no effect on non-started clocks.
 */
void clockUpdate(clock *clock);

/* Starts the provided clock. Resets elapsed time. */
void clockStart(clock *clock);

/* Stops the provided clock. Does not reset elapsed time. */
void clockStop(clock *clock);
