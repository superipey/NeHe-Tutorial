#include "stdafx.h"
#include <gl\GL.h>
#include <gl\GLU.h>

GLvoid ReSizeGLScene(GLsizei width, GLsizei height);
GLvoid KillGLWindow(GLvoid);
BOOL CreateGLWindow(LPCWSTR title, int width, int height, int bits, bool fullscreenflag);
LRESULT CALLBACK WndProc(HWND    hWnd, UINT    uMsg, WPARAM  wParam, LPARAM  lParam);
int WINAPI WinMain(HINSTANCE   hInstance, HINSTANCE   hPrevInstance, LPSTR       lpCmdLine, int nCmdShow);