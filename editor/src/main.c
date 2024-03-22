#include "core/application.h"
#include "core/logger.h"
#include "core/memory.h"
#include "game_types.h"
#include "entry.h"

int main(void) {
    initializeMemory();

    /* Request the game instance from the application. */
    game gameInstance;

    if (!createGame(&gameInstance)) {
        FFATAL("Could not create game!");

        return -1;
    }

    /* Ensure the function pointers exist. */
    if (!gameInstance.render || !gameInstance.update || !gameInstance.initialize || !gameInstance.onResize) {
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

    shutdownMemory();

    return 0;
}
