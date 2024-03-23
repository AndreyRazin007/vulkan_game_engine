#include "platform.h"

/* Windows platform layer. */
#if FPLATFORM_WINDOWS

#include "core/logger.h"
#include "core/input.h"

#include <windows.h>
#include <windowsx.h>
#include <stdlib.h>

typedef struct internalState {
    HINSTANCE hInstance;
    HWND hwnd;
} internalState;

static f64 clockFrequency;
static LARGE_INTEGER startTime;

LRESULT CALLBACK win32ProcessMessage(HWND hwnd, u32 message, WPARAM wParam, LPARAM lParam);

b8 platformStartup(platformState *platformState, const char *applicationName,
                   i32 x, i32 y, i32 width, i32 height) {
    platformState->internalState = malloc(sizeof(internalState));
    internalState *state = (internalState*)platformState->internalState;

    state->hInstance = GetModuleHandleA(0);

    /* Setup and register window class. */
    HICON icon = LoadIcon(state->hInstance, IDI_APPLICATION);
    WNDCLASSA wndClassA;
    memset(&wndClassA, 0, sizeof(wndClassA));
    wndClassA.style = CS_DBLCLKS;
    wndClassA.lpfnWndProc = win32ProcessMessage;
    wndClassA.cbClsExtra = 0;
    wndClassA.cbWndExtra = 0;
    wndClassA.hInstance = state->hInstance;
    wndClassA.hIcon = icon;
    wndClassA.hCursor = LoadCursor(NULL, IDC_ARROW);
    wndClassA.hbrBackground = NULL;
    wndClassA.lpszClassName = "FreakWindowClass";

    if (!RegisterClassA(&wndClassA)) {
        MessageBoxA(0, "Window registration failed", "Error", MB_ICONEXCLAMATION | MB_OK);

        return false;
    }

    /* Create window. */
    u32 clientX = x;
    u32 clientY = y;
    u32 clientWidth = width;
    u32 clientHeight = height;

    u32 windowX = clientX;
    u32 windowY = clientY;
    u32 windowWidth = clientWidth;
    u32 windowHeight = clientHeight;

    u32 windowStyle = WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION;
    u32 windowExStyle = WS_EX_APPWINDOW;

    windowStyle |= WS_MAXIMIZEBOX;
    windowStyle |= WS_MINIMIZEBOX;
    windowStyle |= WS_THICKFRAME;

    /* Obtain the size of the border. */
    RECT border_rect = {0, 0, 0, 0};
    AdjustWindowRectEx(&border_rect, windowStyle, 0, windowExStyle);

    /* In this case, the border rectangle is negative. */
    windowX += border_rect.left;
    windowY += border_rect.top;

    /* Grow by the size of the OS border. */
    windowWidth += border_rect.right - border_rect.left;
    windowHeight += border_rect.bottom - border_rect.top;

    HWND handle = CreateWindowExA(windowExStyle, "FreakWindowClass", applicationName,
                                  windowStyle,
                                  windowX, windowY, windowWidth, windowHeight,
                                  0, 0, state->hInstance, 0);

    if (handle == 0) {
        MessageBoxA(NULL, "Window creation failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
        FFATAL("Window creation failed!");

        return false;
    } else {
        state->hwnd = handle;
    }

    b32 shouldActivate = 1;
    i32 showWindowCommandFlags = shouldActivate ? SW_SHOW : SW_SHOWNOACTIVATE;

    ShowWindow(state->hwnd, showWindowCommandFlags);

    LARGE_INTEGER frequency;
    QueryPerformanceFrequency(&frequency);
    clockFrequency = 1.0 / (f64)frequency.QuadPart;
    QueryPerformanceCounter(&startTime);

    return true;
}

void platformShutdown(platformState *platformState) {
    /* Simply cold-cast to the known type. */
    internalState *state = (internalState*)platformState->internalState;

    if (state->hwnd) {
        DestroyWindow(state->hwnd);
        state->hwnd = 0;
    }
}

b8 platformPumpMessages(platformState *platformState) {
    MSG message;

    while (PeekMessageA(&message, NULL, 0, 0, PM_REMOVE)) {
        TranslateMessage(&message);
        DispatchMessageA(&message);
    }

    return true;
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
    HANDLE console_handle = GetStdHandle(STD_OUTPUT_HANDLE);
    static u8 levels[6] = {64, 4, 6, 2, 1, 8};

    SetConsoleTextAttribute(console_handle, levels[colour]);
    OutputDebugStringA(message);

    u64 length = strlen(message);
    LPDWORD number_written = 0;

    WriteConsoleA(GetStdHandle(STD_OUTPUT_HANDLE), message, (DWORD)length, number_written, 0);
}

void platformConsoleWriteError(const char *message, u8 colour) {
    HANDLE console_handle = GetStdHandle(STD_ERROR_HANDLE);
    static u8 levels[6] = {64, 4, 6, 2, 1, 8};

    SetConsoleTextAttribute(console_handle, levels[colour]);
    OutputDebugStringA(message);

    u64 length = strlen(message);
    LPDWORD number_written = 0;

    WriteConsoleA(GetStdHandle(STD_ERROR_HANDLE), message, (DWORD)length, number_written, 0);
}

f64 platformGetAbsoluteTime() {
    LARGE_INTEGER nowTime;

    QueryPerformanceCounter(&nowTime);

    return (f64)nowTime.QuadPart * clockFrequency;
}

void platformSleep(u64 ms) {
    Sleep(ms);
}

LRESULT CALLBACK win32ProcessMessage(HWND hwnd, u32 message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
        case WM_ERASEBKGND:
            return 1;
        case WM_CLOSE:
            return 0;
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        case WM_SIZE: {} break;
        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
        case WM_KEYUP:
        case WM_SYSKEYUP: {
            b8 pressed = (message == WM_KEYDOWN || message == WM_SYSKEYDOWN);
            keys key = (u16)wParam;

            /* Pass to the input subsystem for processing. */
            inputProcessKey(key, pressed);
        } break;
        case WM_MOUSEMOVE: {
            /* Mouse move. */
            i32 positionX = GET_X_LPARAM(lParam);
            i32 positionY = GET_Y_LPARAM(lParam);

            /* Pass over to the input subsystem. */
            inputProcessMouseMove(positionX, positionY);
        } break;
        case WM_MOUSEWHEEL: {
            i32 deltaZ = GET_WHEEL_DELTA_WPARAM(wParam);

            if (deltaZ != 0) {
                /* Flatten the input to an OS-independent (-1, 1). */
                deltaZ = (deltaZ < 0) ? -1 : 1;
                inputProcessMouseWheel(deltaZ);
            }
        } break;
        case WM_LBUTTONDOWN:
        case WM_MBUTTONDOWN:
        case WM_RBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_MBUTTONUP:
        case WM_RBUTTONUP: {
            b8 pressed = message == WM_LBUTTONDOWN || message == WM_RBUTTONDOWN ||
                         message == WM_MBUTTONDOWN;
            buttons mouseButton = BUTTON_MAX_BUTTONS;
            switch (message) {
                case WM_LBUTTONDOWN:
                case WM_LBUTTONUP:
                    mouseButton = BUTTON_LEFT;
                    break;
                case WM_MBUTTONDOWN:
                case WM_MBUTTONUP:
                    mouseButton = BUTTON_MIDDLE;
                    break;
                case WM_RBUTTONDOWN:
                case WM_RBUTTONUP:
                    mouseButton = BUTTON_RIGHT;
                    break;
            }

            /* Pass over to the input subsystem. */
            if (mouseButton != BUTTON_MAX_BUTTONS) {
                inputProcessButton(mouseButton, pressed);
            }
        } break;
    }

    return DefWindowProcA(hwnd, message, wParam, lParam);
}

#endif /* FPLATFORM_WINDOWS */
