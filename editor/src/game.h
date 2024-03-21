#pragma once

#include <defines.h>
#include <game_types.h>

typedef struct gameState {
    f32 deltaTime;
} gameState;

b8 gameInitialize(game *gameInstance);

b8 gameUpdate(game *gameInstance, f32 deltaTime);

b8 gameRender(game *gameInstance, f32 deltaTime);

void gameOnResize(game *gameInstance, u32 width, u32 height);
