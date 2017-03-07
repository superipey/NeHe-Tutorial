#pragma once

#include "resource.h"
HGLRC           hRC = NULL;                           // Permanent Rendering Context
HDC             hDC = NULL;                           // Private GDI Device Context
HWND            hWnd = NULL;                          // Holds Our Window Handle
HINSTANCE       hInstance;                          // Holds The Instance Of The Application

bool keys[256];
bool active = true;
bool fullscreen = true;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

BOOL CreateGLWindow(LPCWSTR title, int width, int height, int bits, bool fullscreenflag);