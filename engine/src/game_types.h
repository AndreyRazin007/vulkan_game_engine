#pragma once

#include "core/application.h"

/**
 * Represents the basic game state in a game.
 * Called for creation by the application.
 */
typedef struct game {
    application_config app_config;

    b8 (*initialize)(struct game* game_inst);

    b8 (*update)(struct game* game_inst, f32 delta_time);

    b8 (*render)(struct game* game_inst, f32 delta_time);

    void (*on_resize)(struct game* game_inst, u32 width, u32 height);

    void* state;
} game;
