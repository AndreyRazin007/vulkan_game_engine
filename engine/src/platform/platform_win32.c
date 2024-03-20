#include "platform/platform.h"

/* Windows platform layer. */
#if FPLATFORM_WINDOWS

#include "core/logger.h"

#include <windows.h>
#include <windowsx.h>
#include <stdlib.h>

typedef struct internalState {
    HINSTANCE hInstance;
    HWND hwnd;
} internalState;

/* Clock */
static f64 clockFrequency;
static LARGE_INTEGER startTime;

LRESULT CALLBACK win32_process_message(HWND hwnd, u32 message, WPARAM wParam, LPARAM lParam);

b8 platformStartup(platformState *platformState,
                   const char *applicationName,
                   i32 x, i32 y, i32 width, i32 height) {
    platformState->internalState = malloc(sizeof(internalState));
    internalState *state = (internalState *)platformState->internalState;

    state->hInstance = GetModuleHandleA(0);

    /* Setup and register window class. */
    HICON icon = LoadIcon(state->hInstance, IDI_APPLICATION);
    WNDCLASSA wndClassA;
    memset(&wndClassA, 0, sizeof(wndClassA));
    wndClassA.style = CS_DBLCLKS;
    wndClassA.lpfnWndProc = win32_process_message;
    wndClassA.cbClsExtra = 0;
    wndClassA.cbWndExtra = 0;
    wndClassA.hInstance = state->hInstance;
    wndClassA.hIcon = icon;
    wndClassA.hCursor = LoadCursor(NULL, IDC_ARROW);
    wndClassA.hbrBackground = NULL;
    wndClassA.lpszClassName = "FreakWindowClass";

    if (!RegisterClassA(&wndClassA)) {
        MessageBoxA(0, "Window registration failed", "Error", MB_ICONEXCLAMATION | MB_OK);

        return FALSE;
    }

    /* Create window */
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

    HWND handle = CreateWindowExA(windowExStyle, "FreakWindowClass",
                                  applicationName, windowStyle,
                                  windowX, windowY, windowWidth, windowHeight,
                                  0, 0, state->hInstance, 0);

    if (handle == 0) {
        MessageBoxA(NULL, "Window creation failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);

        FFATAL("Window creation failed!");

        return FALSE;
    } else {
        state->hwnd = handle;
    }

    /* Show the window */
    b32 shouldActivate = 1;
    i32 showWindowCommandFlags = shouldActivate ? SW_SHOW : SW_SHOWNOACTIVATE;

    /* If initially minimized, use SW_MINIMIZE : SW_SHOWMINNOACTIVE;
     * If initially maximized, use SW_SHOWMAXIMIZED : SW_MAXIMIZE
     */
    ShowWindow(state->hwnd, showWindowCommandFlags);

    /* Clock setup */
    LARGE_INTEGER frequency;
    QueryPerformanceFrequency(&frequency);
    clockFrequency = 1.0 / (f64)frequency.QuadPart;
    QueryPerformanceCounter(&startTime);

    return TRUE;
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

    return TRUE;
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
    LARGE_INTEGER now_time;
    QueryPerformanceCounter(&now_time);

    return (f64)now_time.QuadPart * clockFrequency;
}

void platformSleep(u64 ms) {
    Sleep(ms);
}

LRESULT CALLBACK win32_process_message(HWND hwnd, u32 message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
        case WM_ERASEBKGND:
            /* Notify the OS that erasing will be handled by the application
             * to prevent flicker.
             */
            return 1;
        case WM_CLOSE:
            /* Fire an event for the application to quit. */
            return 0;
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        case WM_SIZE: {} break;
        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
        case WM_KEYUP:
        case WM_SYSKEYUP: {} break;
        case WM_MOUSEMOVE: {} break;
        case WM_MOUSEWHEEL: {} break;
        case WM_LBUTTONDOWN:
        case WM_MBUTTONDOWN:
        case WM_RBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_MBUTTONUP:
        case WM_RBUTTONUP: {} break;
    }

    return DefWindowProcA(hwnd, message, wParam, lParam);
}

#endif /* FPLATFORM_WINDOWS */
