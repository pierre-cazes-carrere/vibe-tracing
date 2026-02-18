#include "window.h"
#include <stdlib.h>
#include <string.h>
#include <windows.h>

static GameWindow* global_window = NULL;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_CREATE:
            break;
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            EndPaint(hwnd, &ps);
            break;
        }
        case WM_KEYDOWN:
            if (global_window && wParam < 256) {
                global_window->keys[wParam] = 1;
            }
            break;
        case WM_KEYUP:
            if (global_window && wParam < 256) {
                global_window->keys[wParam] = 0;
            }
            break;
        case WM_CLOSE:
        case WM_DESTROY:
            if (global_window) {
                global_window->is_open = 0;
            }
            PostQuitMessage(0);
            break;
        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}

GameWindow* window_create(int width, int height, const char* title) {
    GameWindow* window = (GameWindow*)malloc(sizeof(GameWindow));
    
    window->width = width;
    window->height = height;
    window->is_open = 1;
    window->framebuffer = (uint32_t*)malloc(width * height * sizeof(uint32_t));
    memset(window->keys, 0, sizeof(window->keys));
    
    global_window = window;
    
    // Register window class
    WNDCLASS wc = {0};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = "GameWindow";
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.style = CS_VREDRAW | CS_HREDRAW;
    RegisterClass(&wc);
    
    // Create window
    HWND hwnd = CreateWindowEx(
        0,
        "GameWindow",
        title,
        WS_OVERLAPPEDWINDOW,
        100, 100,
        width, height,
        NULL, NULL,
        GetModuleHandle(NULL),
        NULL
    );
    
    if (!hwnd) {
        free(window->framebuffer);
        free(window);
        return NULL;
    }
    
    window->framebuffer = (uint32_t*)(uintptr_t)hwnd;
    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);
    
    return window;
}

void window_free(GameWindow* window) {
    if (window) {
        if (window->framebuffer) {
            DeleteDC((HDC)window->framebuffer);
        }
        free(window);
    }
}

void window_update(GameWindow* window) {
    if (!window) return;
    
    MSG msg;
    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

void window_draw_frame(GameWindow* window, uint32_t* pixels) {
    if (!window || !pixels) return;
    
    HWND hwnd = (HWND)(uintptr_t)window->framebuffer;
    HDC hdc = GetDC(hwnd);
    
    BITMAPINFO bmi = {0};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = window->width;
    bmi.bmiHeader.biHeight = -window->height;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;
    
    StretchDIBits(hdc, 0, 0, window->width, window->height,
                  0, 0, window->width, window->height,
                  pixels, &bmi, DIB_RGB_COLORS, SRCCOPY);
    
    ReleaseDC(hwnd, hdc);
}

int window_is_open(GameWindow* window) {
    return window ? window->is_open : 0;
}

int window_key_pressed(GameWindow* window, int key) {
    return window && key >= 0 && key < 256 ? window->keys[key] : 0;
}

void window_clear(GameWindow* window, uint32_t color) {
    if (!window || !window->framebuffer) return;
    
    for (int i = 0; i < window->width * window->height; i++) {
        window->framebuffer[i] = color;
    }
}
