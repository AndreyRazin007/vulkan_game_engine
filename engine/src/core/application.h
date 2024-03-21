#pragma once

#include "defines.h"

struct game;

/* Application configuration. */
typedef struct applicationConfig {
    /* Window starting position x axis, if applicable. */
    i16 startPositionX;

    /* Window starting position y axis, if applicable. */
    i16 startPositionY;

    /* Window starting width, if applicable. */
    i16 startWidth;

    /* Window starting height, if applicable. */
    i16 startHeight;

    /* The application name used in windowing, if applicable. */
    char *name;
} applicationConfig;


FAPI b8 applicationCreate(struct game *gameInstance);

FAPI b8 applicationRun();
