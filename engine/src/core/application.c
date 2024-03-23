#include "application.h"
#include "game_types.h"
#include "logger.h"
#include "platform/platform.h"
#include "core/memory.h"
#include "core/event.h"
#include "core/input.h"

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

/* Event handlers. */
b8 applicationOnEvent(u16 code, void* sender, void* listenerInstance, eventContext context);
b8 applicationOnKey(u16 code, void* sender, void* listenerInstance, eventContext context);

b8 applicationCreate(game *gameInstance) {
    if (initialized) {
        FERROR("applicationCreate called more than once.");

        return false;
    }

    appState.gameInstance = gameInstance;

    /* Initialize subsystems. */
    initializeLogging();
    inputInitialize();

    FFATAL("A test message: %f", 3.14f);
    FERROR("A test message: %f", 3.14f);
    FWARNING("A test message: %f", 3.14f);
    FINFO("A test message: %f", 3.14f);
    FDEBUG("A test message: %f", 3.14f);
    FTRACE("A test message: %f", 3.14f);

    appState.isRunning = true;
    appState.isSuspended = false;

    if(!eventInitialize()) {
        FERROR("Event system failed initialization. Application cannot continue.");

        return false;
    }

    eventRegister(EVENT_CODE_APPLICATION_QUIT, 0, applicationOnEvent);
    eventRegister(EVENT_CODE_KEY_PRESSED, 0, applicationOnKey);
    eventRegister(EVENT_CODE_KEY_RELEASED, 0, applicationOnKey);

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

    appState.gameInstance->onResize(appState.gameInstance, appState.width, appState.height);

    initialized = true;

    return true;
}

b8 applicationRun() {
    FINFO(getMemoryUsageStr());

    while (appState.isRunning) {
        if (!platformPumpMessages(&appState.platform)) {
            appState.isRunning = false;
        }

        if (!appState.isSuspended) {
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

            inputUpdate(0);
        }
    }

    appState.isRunning = false;

    /* Shutdown event system. */
    eventUnregister(EVENT_CODE_APPLICATION_QUIT, 0, applicationOnEvent);
    eventUnregister(EVENT_CODE_KEY_PRESSED, 0, applicationOnKey);
    eventUnregister(EVENT_CODE_KEY_RELEASED, 0, applicationOnKey);

    eventShutdown();
    inputShutdown();

    platformShutdown(&appState.platform);

    return true;
}

b8 applicationOnEvent(u16 code, void *sender, void *listenerInstance, eventContext context) {
    switch (code) {
        case EVENT_CODE_APPLICATION_QUIT: {
            FINFO("EVENT_CODE_APPLICATION_QUIT recieved, shutting down.\n");

            appState.isRunning = false;
            return true;
        }
    }

    return false;
}

b8 applicationOnKey(u16 code, void *sender, void *listenerInstance, eventContext context) {
    if (code == EVENT_CODE_KEY_PRESSED) {
        u16 keyCode = context.data.u16[0];
        if (keyCode == KEY_ESCAPE) {
            /* Technically firing an event to itself, but there may be other listeners. */
            eventContext data = {};
            eventFire(EVENT_CODE_APPLICATION_QUIT, 0, data);

            /* Block anything else from processing this. */
            return true;
        } else if (keyCode == KEY_A) {
            /* Example on checking for a key. */
            FDEBUG("Explicit - A key pressed!");
        } else {
            FDEBUG("'%c' key pressed in window.", keyCode);
        }
    } else if (code == EVENT_CODE_KEY_RELEASED) {
        u16 keyCode = context.data.u16[0];
        if (keyCode == KEY_B) {
            /* Example on checking for a key. */
            FDEBUG("Explicit - B key released!");
        } else {
            FDEBUG("'%c' key released in window.", keyCode);
        }
    }
    return false;
}
