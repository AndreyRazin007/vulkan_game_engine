#include "game.h"

#include "entry.h"

#include "platform/platform.h"

/* Define the function to create a game */
b8 createGame(game *outGame) {
    // Application configuration.
    outGame->appConfig.startPositionX = 100;
    outGame->appConfig.startPositionY = 100;
    outGame->appConfig.startWidth = 1280;
    outGame->appConfig.startHeight = 720;
    outGame->appConfig.name = "Freak Engine Editor";
    outGame->update = gameUpdate;
    outGame->render = gameRender;
    outGame->initialize = gameInitialize;
    outGame->on_resize = gameOnResize;

    /* Create the game state. */
    outGame->state = platformAllocate(sizeof(gameState), false);

    return true;
}