#include "game.h"

#include <core/logger.h>

b8 gameInitialize(game *gameInstance) {
    FDEBUG("gameInitialize() called!");

    return true;
}

b8 gameUpdate(game *gameInstance, f32 deltaTime) {
    return true;
}

b8 gameRender(game *gameInstance, f32 deltaTime) {
    return true;
}

void gameOnResize(game *gameInstance, u32 width, u32 height) {}