#include "platform.h"

/* Linux platform layer. */
#if FPLATFORM_LINUX

#include "core/logger.h"

#include <xcb/xcb.h>
#include <X11/keysym.h>
#include <X11/XKBlib.h>
#include <X11/Xlib.h>
#include <X11/Xlib-xcb.h>
#include <sys/time.h>

#if _POSIX_C_SOURCE >= 199309L
#include <time.h> /* nanosleep */
#else
#include <unistd.h> /* usleep */
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct internalState {
    Display *display;
    xcb_connection_t *connection;
    xcb_window_t window;
    xcb_screen_t *screen;
    xcb_atom_t wmProtocols;
    xcb_atom_t wmDeleteWindow;
} internalState;

b8 platformStartup(platformState *platformState, const char *applicationName,
                   i32 x, i32 y, i32 width, i32 height) {
    /* Create the internal state. */
    platformState->internalState = malloc(sizeof(internalState));
    internalState *state = (internalState*)platformState->internalState;

    /* Connect to X */
    state->display = XOpenDisplay(NULL);

    /* Turn off key repeats. */
    XAutoRepeatOff(state->display);

    /* Retrieve the connection from the display. */
    state->connection = XGetXCBConnection(state->display);

    if (xcb_connection_has_error(state->connection)) {
        FFATAL("Failed to connect to X server via XCB.");

        return false;
    }

    /* Get data from the X server */
    const struct xcb_setup_t *setup = xcb_get_setup(state->connection);

    /* Loop through screens using iterator */
    xcb_screen_iterator_t iterator = xcb_setup_roots_iterator(setup);
    int screen_p = 0;
    for (i32 screen = screen_p; screen > 0; --screen) {
        xcb_screen_next(&iterator);
    }

    /* After screens have been looped through, assign it. */
    state->screen = iterator.data;

    /* Allocate a XID for the window to be created. */
    state->window = xcb_generate_id(state->connection);

    /* Register event types.
     * XCB_CW_BACK_PIXEL = filling then window bg with a single colour
     * XCB_CW_EVENT_MASK is required.
     */
    u32 eventMask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;

    /* Listen for keyboard and mouse buttons */
    u32 eventValues = XCB_EVENT_MASK_BUTTON_PRESS | XCB_EVENT_MASK_BUTTON_RELEASE |
                       XCB_EVENT_MASK_KEY_PRESS | XCB_EVENT_MASK_KEY_RELEASE |
                       XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_POINTER_MOTION |
                       XCB_EVENT_MASK_STRUCTURE_NOTIFY;

    /* Values to be sent over XCB (bg colour, events) */
    u32 valueList[] = {state->screen->black_pixel, eventValues};

    /* Create the window */
    xcb_void_cookie_t cookie = xcb_create_window(state->connection,
                                                 XCB_COPY_FROM_PARENT, state->window,
                                                 state->screen->root, x, y, width,
                                                 height, 0, XCB_WINDOW_CLASS_INPUT_OUTPUT,
                                                 state->screen->root_visual, eventMask,
                                                 valueList);

    xcb_change_property(state->connection, XCB_PROP_MODE_REPLACE, state->window,
                        XCB_ATOM_WM_NAME, XCB_ATOM_STRING, 8, strlen(applicationName),
                        applicationName);

    /* Tell the server to notify when the window manager
     * attempts to destroy the window.
     */
    xcb_intern_atom_cookie_t wmDeleteCookie = xcb_intern_atom(state->connection, 0,
                                                                strlen("WM_DELETE_WINDOW"),
                                                                "WM_DELETE_WINDOW");
    xcb_intern_atom_cookie_t wmProtocolsCookie = xcb_intern_atom(state->connection, 0,
                                                                 strlen("WM_PROTOCOLS"),
                                                                 "WM_PROTOCOLS");
    xcb_intern_atom_reply_t *wmDeleteReply = xcb_intern_atom_reply(state->connection,
                                                                   wmDeleteCookie, NULL);
    xcb_intern_atom_reply_t *wmProtocolsReply = xcb_intern_atom_reply(state->connection,
                                                                      wmProtocolsCookie,
                                                                      NULL);
    state->wmDeleteWindow = wmDeleteReply->atom;
    state->wmProtocols = wmProtocolsReply->atom;

    xcb_change_property(state->connection, XCB_PROP_MODE_REPLACE, state->window,
                        wmProtocolsReply->atom, 4, 32, 1, &wmDeleteReply->atom);

    /* Map the window to the screen */
    xcb_map_window(state->connection, state->window);

    /* Flush the stream */
    i32 streamResult = xcb_flush(state->connection);
    if (streamResult <= 0) {
        FFATAL("An error occurred when flusing the stream: %d", streamResult);

        return false;
    }

    return true;
}

void platformShutdown(platformState *platformState) {
    /* Simply cold-cast to the known type. */
    internalState *state = (internalState*)platformState->internalState;

    /* Turn key repeats back on since this is global for the OS... just... wow. */
    XAutoRepeatOn(state->display);

    xcb_destroy_window(state->connection, state->window);
}

b8 platformPumpMessages(platformState *platformState) {
    /* Simply cold-cast to the known type. */
    internalState *state = (internalState*)platformState->internalState;

    xcb_generic_event_t *event;
    xcb_client_message_event_t *clientMessage;

    b8 quitFlagged = false;

    /* Poll for events until null is returned. */
    while (event != 0) {
        event = xcb_poll_for_event(state->connection);
        if (event == 0) {
            break;
        }

        /* Input events */
        switch (event->response_type & ~0x80) {
            case XCB_KEY_PRESS:
            case XCB_KEY_RELEASE: {} break;
            case XCB_BUTTON_PRESS:
            case XCB_BUTTON_RELEASE: {}
            case XCB_MOTION_NOTIFY:
                break;

            case XCB_CONFIGURE_NOTIFY: {}

            case XCB_CLIENT_MESSAGE: {
                clientMessage = (xcb_client_message_event_t*)event;

                /* Window close */
                if (clientMessage->data.data32[0] == state->wmDeleteWindow) {
                    quitFlagged = true;
                }
            } break;
            default:
                break;
        }

        free(event);
    }

    return !quitFlagged;
}

void *platformAllocate(u64 size, b8 aligned) {
    return malloc(size);
}

void platformFree(void *block, b8 aligned) {
    free(block);
}

void *platformZeroMemory(void *block, u64 size) {
    return memset(block, 0, size);
}

void *platformCopyMemory(void *dest, const void *source, u64 size) {
    return memcpy(dest, source, size);
}

void *platformSetMemory(void *dest, i32 value, u64 size) {
    return memset(dest, value, size);
}

void platformConsoleWrite(const char *message, u8 colour) {
    const char *colourStrings[] = {"0;41", "1;31", "1;33", "1;32", "1;34", "1;30"};
    printf("\033[%sm%s\033[0m", colourStrings[colour], message);
}

void platformConsoleWriteError(const char* message, u8 colour) {
    const char *colourStrings[] = {"0;41", "1;31", "1;33", "1;32", "1;34", "1;30"};
    printf("\033[%sm%s\033[0m", colourStrings[colour], message);
}

f64 platformGetAbsoluteTime() {
    struct timespec now;
    clock_gettime(1, &now);

    return now.tv_sec + now.tv_nsec * 0.000000001;
}

void platformSleep(u64 ms) {
#if _POSIX_C_SOURCE >= 199309L
    struct timespec timeSpec;
    timeSpec.tv_sec = ms / 1000;
    timeSpec.tv_nsec = (ms % 1000) * 1000 * 1000;
    nanosleep(&timeSpec, 0);
#else
    if (ms >= 1000) {
        sleep(ms / 1000);
    }

    usleep((ms % 1000) * 1000);
#endif
}

#endif
