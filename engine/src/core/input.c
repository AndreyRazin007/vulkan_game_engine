#include "core/input.h"
#include "core/event.h"
#include "core/memory.h"
#include "core/logger.h"

typedef struct keyboardState {
    b8 keys[256];
} keyboardState;

typedef struct mouseState {
    i16 x;
    i16 y;
    u8 buttons[BUTTON_MAX_BUTTONS];
} mouseState;

typedef struct inputState {
    keyboardState keyboardCurrent;
    keyboardState keyboardPrevious;
    mouseState mouseCurrent;
    mouseState mousePrevious;
} inputState;

/* Internal input state. */
static b8 initialized = false;
static inputState state = {};

void inputInitialize() {
    zeroMemory(&state, sizeof(inputState));
    initialized = true;

    FINFO("Input subsystem initialized.");
}

void inputShutdown() {
    /* Add shutdown routines when needed. */
    initialized = false;
}

void inputUpdate(f64 deltaTime) {
    if (!initialized) {
        return;
    }

    /* Copy current states to previous states. */
    copyMemory(&state.keyboardPrevious, &state.keyboardCurrent, sizeof(keyboardState));
    copyMemory(&state.mousePrevious, &state.mouseCurrent, sizeof(mouseState));
}

void inputProcessKey(keys key, b8 pressed) {
    /* Only handle this if the state actually changed. */
    if (state.keyboardCurrent.keys[key] != pressed) {
        /* Update internal state. */
        state.keyboardCurrent.keys[key] = pressed;

        /* Fire off an event for immediate processing. */
        eventContext context;
        context.data.u16[0] = key;
        eventFire(pressed ? EVENT_CODE_KEY_PRESSED : EVENT_CODE_KEY_RELEASED, 0, context);
    }
}

void inputProcessButton(buttons button, b8 pressed) {
    /* If the state changed, fire an event. */
    if (state.mouseCurrent.buttons[button] != pressed) {
        state.mouseCurrent.buttons[button] = pressed;

        /* Fire the event. */
        eventContext context;
        context.data.u16[0] = button;
        eventFire(pressed ? EVENT_CODE_BUTTON_PRESSED : EVENT_CODE_BUTTON_RELEASED, 0, context);
    }
}

void inputProcessMouseMove(i16 x, i16 y) {
    /* Only process if actually different. */
    if (state.mouseCurrent.x != x || state.mouseCurrent.y != y) {
        /* Update internal state. */
        state.mouseCurrent.x = x;
        state.mouseCurrent.y = y;

        /* Fire the event. */
        eventContext context;
        context.data.u16[0] = x;
        context.data.u16[1] = y;
        eventFire(EVENT_CODE_MOUSE_MOVED, 0, context);
    }
}

void inputProcessMouseWheel(i8 z_delta) {
    /* Fire the event. */
    eventContext context;
    context.data.u8[0] = z_delta;
    eventFire(EVENT_CODE_MOUSE_WHEEL, 0, context);
}

b8 inputIsKeyDown(keys key) {
    if (!initialized) {
        return false;
    }

    return state.keyboardCurrent.keys[key] == true;
}

b8 inputIsKeyUp(keys key) {
    if (!initialized) {
        return true;
    }

    return state.keyboardCurrent.keys[key] == false;
}

b8 inputWasKeyDown(keys key) {
    if (!initialized) {
        return false;
    }

    return state.keyboardPrevious.keys[key] == true;
}

b8 inputWasKeyUp(keys key) {
    if (!initialized) {
        return true;
    }

    return state.keyboardPrevious.keys[key] == false;
}

// mouse input
b8 inputIsButtonDown(buttons button) {
    if (!initialized) {
        return false;
    }

    return state.mouseCurrent.buttons[button] == true;
}

b8 inputIsButtonUp(buttons button) {
    if (!initialized) {
        return true;
    }

    return state.mouseCurrent.buttons[button] == false;
}

b8 inputWasButtonDown(buttons button) {
    if (!initialized) {
        return false;
    }

    return state.mousePrevious.buttons[button] == true;
}

b8 inputWasButtonUp(buttons button) {
    if (!initialized) {
        return true;
    }

    return state.mousePrevious.buttons[button] == false;
}

void inputGetMousePosition(i32 *x, i32 *y) {
    if (!initialized) {
        *x = 0;
        *y = 0;

        return;
    }

    *x = state.mouseCurrent.x;
    *y = state.mouseCurrent.y;
}

void inputGetPreviousMousePosition(i32 *x, i32 *y) {
    if (!initialized) {
        *x = 0;
        *y = 0;

        return;
    }

    *x = state.mousePrevious.x;
    *y = state.mousePrevious.y;
}
