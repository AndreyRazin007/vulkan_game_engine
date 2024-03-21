#pragma once

#include "core/application.h"
#include "core/logger.h"
#include "game_types.h"

/* Externally-defined function to create a game. */
extern b8 createGame(game *outGame);

/*
 * The main entry point of the application.
 */
int main(void) {
    /* Request the game instance from the application. */
    game gameInstance;

    if (!createGame(&gameInstance)) {
        FFATAL("Could not create game!");

        return -1;
    }

    /* Ensure the function pointers exist. */
    if (!gameInstance.render || !gameInstance.update || !gameInstance.initialize || !gameInstance.on_resize) {
        FFATAL("The game's function pointers must be assigned!");

        return -2;
    }

    /* Initialization. */
    if (!applicationCreate(&gameInstance)) {
        FINFO("Application failed to create!.");

        return 1;
    }

    /* Begin the game loop. */
    if(!applicationRun()) {
        FINFO("Application did not shutdown gracefully.");

        return 2;
    }

    return 0;
}