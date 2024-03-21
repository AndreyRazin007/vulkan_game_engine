#include "application.h"
#include "game_types.h"

#include "logger.h"

#include "platform/platform.h"

typedef struct applicationState {
    game *gameInstance;
    b8 isRunning;
    b8 isSuspended;
    platformState platform;
    i16 width;
    i16 height;
    f64 lastTime;
} applicationState;

static b8 initialized = false;
static applicationState appState;

b8 applicationCreate(game *gameInstance) {
    if (initialized) {
        FERROR("applicationCreate called more than once.");

        return false;
    }

    appState.gameInstance = gameInstance;

    /* Initialize subsystems. */
    initializeLogging();

    FFATAL("A test message: %f", 3.14f);
    FERROR("A test message: %f", 3.14f);
    FWARNING("A test message: %f", 3.14f);
    FINFO("A test message: %f", 3.14f);
    FDEBUG("A test message: %f", 3.14f);
    FTRACE("A test message: %f", 3.14f);

    appState.isRunning = true;
    appState.isSuspended = false;

    if (!platformStartup(&appState.platform, gameInstance->appConfig.name,
                         gameInstance->appConfig.startPositionX,
                         gameInstance->appConfig.startPositionY,
                         gameInstance->appConfig.startWidth,
                         gameInstance->appConfig.startHeight)) {
        return false;
    }

    /* Initialize the game. */
    if (!appState.gameInstance->initialize(appState.gameInstance)) {
        FFATAL("Game failed to initialize.");

        return false;
    }

    appState.gameInstance->on_resize(appState.gameInstance, appState.width, appState.height);

    initialized = true;
    
    return true;
}

b8 applicationRun() {
    while (appState.isRunning) {
        if(!platformPumpMessages(&appState.platform)) {
            appState.isRunning = false;
        }

        if(!appState.isSuspended) {
            if (!appState.gameInstance->update(appState.gameInstance, (f32)0)) {
                FFATAL("Game update failed, shutting down.");
                appState.isRunning = false;
                break;
            }

            /* Call the game's render routine. */
            if (!appState.gameInstance->render(appState.gameInstance, (f32)0)) {
                FFATAL("Game render failed, shutting down.");
                appState.isRunning = false;
                break;
            }
        }
    }

    appState.isRunning = false;

    platformShutdown(&appState.platform);

    return true;
}