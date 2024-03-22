#include "core/event.h"

#include "core/memory.h"
#include "containers/darray.h"

typedef struct registeredEvent {
    void *listener;
    PFNOnEvent callback;
} registeredEvent;

typedef struct eventCodeEntry {
    registeredEvent *events;
} eventCodeEntry;

/* This should be more than enough codes... */
#define MAX_MESSAGE_CODES 16384

/* State structure. */
typedef struct eventSystemState {
    /* Lookup table for event codes. */
    eventCodeEntry registered[MAX_MESSAGE_CODES];
} eventSystemState;

/*
 * Event system internal state.
 */
static b8 isInitialized = false;
static eventSystemState state;

b8 eventInitialize() {
    if (isInitialized == true) {
        return false;
    }
    isInitialized = false;
    zeroMemory(&state, sizeof(state));

    isInitialized = true;

    return true;
}

void eventShutdown() {
    /* Free the events arrays. And objects pointed to should be destroyed on their own. */
    for (u16 i = 0; i < MAX_MESSAGE_CODES; ++i) {
        if (state.registered[i].events != 0) {
            darrayDestroy(state.registered[i].events);
            state.registered[i].events = 0;
        }
    }
}

b8 eventRegister(u16 code, void *listener, PFNOnEvent onEvent) {
    if (isInitialized == false) {
        return false;
    }

    if (state.registered[code].events == 0) {
        state.registered[code].events = darrayCreate(registeredEvent);
    }

    u64 registeredCount = darrayLength(state.registered[code].events);
    for (u64 i = 0; i < registeredCount; ++i) {
        if(state.registered[code].events[i].listener == listener) {
            return false;
        }
    }

    /* If at this point, no duplicate was found. Proceed with registration. */
    registeredEvent event;
    event.listener = listener;
    event.callback = onEvent;
    darrayPush(state.registered[code].events, event);

    return true;
}

b8 eventUnregister(u16 code, void *listener, PFNOnEvent onEvent) {
    if(isInitialized == false) {
        return false;
    }

    /* On nothing is registered for the code, boot out. */
    if (state.registered[code].events == 0) {
        return false;
    }

    u64 registeredCount = darrayLength(state.registered[code].events);
    for(u64 i = 0; i < registeredCount; ++i) {
        registeredEvent event = state.registered[code].events[i];
        if(event.listener == listener && event.callback == onEvent) {
            /* Found one, remove it. */
            registeredEvent poppedEvent;
            darrayPopAt(state.registered[code].events, i, &poppedEvent);

            return true;
        }
    }

    /* Not found. */
    return false;
}

b8 eventFire(u16 code, void *sender, eventContext context) {
    if (isInitialized == false) {
        return false;
    }

    /* If nothing is registered for the code, boot out. */
    if(state.registered[code].events == 0) {
        return false;
    }

    u64 registeredCount = darrayLength(state.registered[code].events);
    for(u64 i = 0; i < registeredCount; ++i) {
        registeredEvent event = state.registered[code].events[i];
        if(event.callback(code, sender, event.listener, context)) {
            /* Message has been handled, do not send to other listeners. */
            return true;
        }
    }

    /* Not found. */
    return false;
}
