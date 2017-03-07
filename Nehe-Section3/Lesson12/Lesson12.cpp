// Lesson06.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "Lesson12.h"
#include <gl\GL.h>
#include <gl\GLU.h>
#include <stdio.h>
#include "SOIL.h"
#include "GLWindow.h"

#pragma comment(lib,"opengl32.lib")
#pragma comment(lib,"glu32.lib")
#pragma comment(lib,"SOIL.lib")

HDC     hDC = NULL;                           // Private GDI Device Context
HGLRC       hRC = NULL;                           // Permanent Rendering Context
HWND        hWnd = NULL;                          // Holds Our Window Handle
HINSTANCE   hInstance;                          // Holds The Instance Of The Application

bool        keys[256];                          // Array Used For The Keyboard Routine
bool        active = TRUE;                            // Window Active Flag
bool        fullscreen = TRUE;                        // Fullscreen Flag

GLuint  texture[1];                                 // Storage For One Texture
GLuint  box;                                        // Storage For The Display List
GLuint  top;                                        // Storage For The Second Display List
GLuint  xloop;                                      // Loop For X Axis
GLuint  yloop;                                      // Loop For Y Axis

GLfloat xrot;                                       // Rotates Cube On The X Axis
GLfloat yrot;                                       // Rotates Cube On The Y Axis

static GLfloat boxcol[5][3] =                                // Array For Box Colors
{
	// Bright:  Red, Orange, Yellow, Green, Blue
	{ 1.0f,0.0f,0.0f },{ 1.0f,0.5f,0.0f },{ 1.0f,1.0f,0.0f },{ 0.0f,1.0f,0.0f },{ 0.0f,1.0f,1.0f }
};

static GLfloat topcol[5][3] =                                // Array For Top Colors
{
	// Dark:  Red, Orange, Yellow, Green, Blue
	{ .5f,0.0f,0.0f },{ 0.5f,0.25f,0.0f },{ 0.5f,0.5f,0.0f },{ 0.0f,0.5f,0.0f },{ 0.0f,0.5f,0.5f }
};

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);               // Declaration For WndProc

GLvoid BuildLists()                                 // Build Box Display List
{
	box = glGenLists(2);                              // Building Two Lists
	glNewList(box, GL_COMPILE);                          // New Compiled box Display List

	glBegin(GL_QUADS);                          // Start Drawing Quads
												// Bottom Face
	glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f, -1.0f, -1.0f);  // Top Right Of The Texture and Quad
	glTexCoord2f(0.0f, 1.0f); glVertex3f(1.0f, -1.0f, -1.0f);  // Top Left Of The Texture and Quad
	glTexCoord2f(0.0f, 0.0f); glVertex3f(1.0f, -1.0f, 1.0f);  // Bottom Left Of The Texture and Quad
	glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f, 1.0f);  // Bottom Right Of The Texture and Quad
															   // Front Face
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f, 1.0f);  // Bottom Left Of The Texture and Quad
	glTexCoord2f(1.0f, 0.0f); glVertex3f(1.0f, -1.0f, 1.0f);  // Bottom Right Of The Texture and Quad
	glTexCoord2f(1.0f, 1.0f); glVertex3f(1.0f, 1.0f, 1.0f);  // Top Right Of The Texture and Quad
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f, 1.0f, 1.0f);  // Top Left Of The Texture and Quad
															  // Back Face
	glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);  // Bottom Right Of The Texture and Quad
	glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f, 1.0f, -1.0f);  // Top Right Of The Texture and Quad
	glTexCoord2f(0.0f, 1.0f); glVertex3f(1.0f, 1.0f, -1.0f);  // Top Left Of The Texture and Quad
	glTexCoord2f(0.0f, 0.0f); glVertex3f(1.0f, -1.0f, -1.0f);  // Bottom Left Of The Texture and Quad
															   // Right face
	glTexCoord2f(1.0f, 0.0f); glVertex3f(1.0f, -1.0f, -1.0f);  // Bottom Right Of The Texture and Quad
	glTexCoord2f(1.0f, 1.0f); glVertex3f(1.0f, 1.0f, -1.0f);  // Top Right Of The Texture and Quad
	glTexCoord2f(0.0f, 1.0f); glVertex3f(1.0f, 1.0f, 1.0f);  // Top Left Of The Texture and Quad
	glTexCoord2f(0.0f, 0.0f); glVertex3f(1.0f, -1.0f, 1.0f);  // Bottom Left Of The Texture and Quad
															  // Left Face
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);  // Bottom Left Of The Texture and Quad
	glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f, 1.0f);  // Bottom Right Of The Texture and Quad
	glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f, 1.0f, 1.0f);  // Top Right Of The Texture and Quad
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f, 1.0f, -1.0f);  // Top Left Of The Texture and Quad
	glEnd();

	glEndList();                                    // Done Building The box List
	top = box + 1;                                  // top List Value Is box List Value +1

	glNewList(top, GL_COMPILE);                          // New Compiled top Display List

	glBegin(GL_QUADS);                          // Start Drawing Quad
												// Top Face
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f, 1.0f, -1.0f);  // Top Left Of The Texture and Quad
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, 1.0f, 1.0f);  // Bottom Left Of The Texture and Quad
	glTexCoord2f(1.0f, 0.0f); glVertex3f(1.0f, 1.0f, 1.0f);  // Bottom Right Of The Texture and Quad
	glTexCoord2f(1.0f, 1.0f); glVertex3f(1.0f, 1.0f, -1.0f);  // Top Right Of The Texture and Quad
	glEnd();
	glEndList();                                    // Done Building The top Display List
}

int LoadGLTextures()                                    // Load Bitmaps And Convert To Textures
{
	/* load an image file directly as a new OpenGL texture */
	texture[0] = SOIL_load_OGL_texture
	(
		"Data/Cube.bmp",
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_INVERT_Y
	);

	if (texture[0] == 0)
		return false;


	// Typical Texture Generation Using Data From The Bitmap
	glBindTexture(GL_TEXTURE_2D, texture[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	return true;                                        // Return Success
}

int InitGL(GLvoid)                              // All Setup For OpenGL Goes Here
{
	if (!LoadGLTextures())                              // Jump To Texture Loading Routine
	{
		return FALSE;                               // If Texture Didn't Load Return FALSE
	}
	BuildLists();                                   // Jump To The Code That Creates Our Display Lists
	glEnable(GL_TEXTURE_2D);                            // Enable Texture Mapping
	glShadeModel(GL_SMOOTH);                            // Enable Smooth Shading
	glClearColor(0.0f, 0.0f, 0.0f, 0.5f);                       // Black Background
	glClearDepth(1.0f);                             // Depth Buffer Setup
	glEnable(GL_DEPTH_TEST);                            // Enables Depth Testing
	glDepthFunc(GL_LEQUAL);                             // The Type Of Depth Testing To Do

	glEnable(GL_LIGHT0);                                // Quick And Dirty Lighting (Assumes Light0 Is Set Up)
	glEnable(GL_LIGHTING);                              // Enable Lighting
	glEnable(GL_COLOR_MATERIAL);                            // Enable Material Coloring
	
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);              // Nice Perspective Correction
	return TRUE;
}

int DrawGLScene(GLvoid)                             // Here's Where We Do All The Drawing
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);             // Clear The Screen And The Depth Buffer

	glBindTexture(GL_TEXTURE_2D, texture[0]);                   // Select The Texture

	for (yloop = 1; yloop<6; yloop++)                            // Loop Through The Y Plane
	{
		for (xloop = 0; xloop<yloop; xloop++)                    // Loop Through The X Plane
		{
			glLoadIdentity();                       // Reset The View
													// Position The Cubes On The Screen
			glTranslatef(1.4f + (float(xloop)*2.8f) - (float(yloop)*1.4f), ((6.0f - float(yloop))*2.4f) - 7.0f, -20.0f);
			glRotatef(45.0f - (2.0f*yloop) + xrot, 1.0f, 0.0f, 0.0f);      // Tilt The Cubes Up And Down
			glRotatef(45.0f + yrot, 0.0f, 1.0f, 0.0f);               // Spin Cubes Left And Right
			glCallList(box);                        // Draw The Box

			glColor3fv(topcol[yloop - 1]);                    // Select The Top Color
			glCallList(top);                        // Draw The Top
		}
	}

	return true;                                // Keep Going
}

// Windows Operation
GLvoid ReSizeGLScene(GLsizei width, GLsizei height) {
	if (height == 0) height = 1;
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

GLvoid KillGLWindow(GLvoid)                         // Properly Kill The Window
{
	if (fullscreen) {
		ChangeDisplaySettings(NULL, 0);                  // If So Switch Back To The Desktop
		ShowCursor(true);
	}

	if (hRC) {
		if (!wglMakeCurrent(NULL, NULL)) {
			MessageBox(NULL, L"Release Of DC And RC Failed.", L"SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
		}


		if (!wglDeleteContext(hRC))                 // Are We Able To Delete The RC?
		{
			MessageBox(NULL, L"Release Rendering Context Failed.", L"SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
		}
		hRC = NULL;
	}

	if (hDC && !ReleaseDC(hWnd, hDC))                    // Are We Able To Release The DC
	{
		MessageBox(NULL, L"Release Device Context Failed.", L"SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
		hDC = NULL;
	}

	if (hWnd && !DestroyWindow(hWnd))                   // Are We Able To Destroy The Window?
	{
		MessageBox(NULL, L"Could Not Release hWnd.", L"SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
		hWnd = NULL;
	}

	if (!UnregisterClass(L"OpenGL", hInstance))               // Are We Able To Unregister Class
	{
		MessageBox(NULL, L"Could Not Unregister Class.", L"SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
		hInstance = NULL;
	}
}

BOOL CreateGLWindow(LPCWSTR title, int width, int height, int bits, bool fullscreenflag)
{
	GLuint      PixelFormat;                        // Holds The Results After Searching For A Match
	WNDCLASS    wc;                         // Windows Class Structure
	DWORD       dwExStyle;                      // Window Extended Style
	DWORD       dwStyle;                        // Window Style
	RECT WindowRect;                            // Grabs Rectangle Upper Left / Lower Right Values
	WindowRect.left = (long)0;                        // Set Left Value To 0
	WindowRect.right = (long)width;                       // Set Right Value To Requested Width
	WindowRect.top = (long)0;                         // Set Top Value To 0
	WindowRect.bottom = (long)height;                     // Set Bottom Value To Requested Height
	fullscreen = fullscreenflag;                      // Set The Global Fullscreen Flag
	hInstance = GetModuleHandle(NULL);            // Grab An Instance For Our Window
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;       // Redraw On Move, And Own DC For Window
	wc.lpfnWndProc = (WNDPROC)WndProc;                // WndProc Handles Messages
	wc.cbClsExtra = 0;                        // No Extra Window Data
	wc.cbWndExtra = 0;                        // No Extra Window Data
	wc.hInstance = hInstance;                    // Set The Instance
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);          // Load The Default Icon
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);          // Load The Arrow Pointer
	wc.hbrBackground = NULL;                     // No Background Required For GL
	wc.lpszMenuName = NULL;                     // We Don't Want A Menu
	wc.lpszClassName = L"OpenGL";                 // Set The Class Name

	if (!RegisterClass(&wc))                        // Attempt To Register The Window Class
	{
		MessageBox(NULL, L"Failed To Register The Window Class.", L"ERROR", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;                           // Exit And Return FALSE
	}

	if (fullscreen)                             // Attempt Fullscreen Mode?
	{
		DEVMODE dmScreenSettings;                   // Device Mode
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));       // Makes Sure Memory's Cleared
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);       // Size Of The Devmode Structure
		dmScreenSettings.dmPelsWidth = width;            // Selected Screen Width
		dmScreenSettings.dmPelsHeight = height;           // Selected Screen Height
		dmScreenSettings.dmBitsPerPel = bits;             // Selected Bits Per Pixel
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		// Try To Set Selected Mode And Get Results.  NOTE: CDS_FULLSCREEN Gets Rid Of Start Bar.
		if (ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
		{
			if (MessageBox(NULL, L"The Requested Fullscreen Mode Is Not Supported By\nYour Video Card. Use Windowed Mode Instead?", L"NeHe GL", MB_YESNO | MB_ICONEXCLAMATION) == IDYES)
			{
				fullscreen = FALSE;               // Select Windowed Mode (Fullscreen=FALSE)
			}
			else
			{// Pop Up A Message Box Letting User Know The Program Is Closing.
				MessageBox(NULL, L"Program Will Now Close.", L"ERROR", MB_OK | MB_ICONSTOP);
				return FALSE;                   // Exit And Return FALSE
			}
		}
	}

	if (fullscreen)                             // Are We Still In Fullscreen Mode?
	{
		dwExStyle = WS_EX_APPWINDOW;                  // Window Extended Style
		dwStyle = WS_POPUP;                       // Windows Style
		ShowCursor(FALSE);                      // Hide Mouse Pointer
	}
	else
	{
		dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;           // Window Extended Style
		dwStyle = WS_OVERLAPPEDWINDOW;                    // Windows Style
	}

	AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle);     // Adjust Window To True Requested Size

	if (!(hWnd = CreateWindowEx(dwExStyle,              // Extended Style For The Window
		L"OpenGL",               // Class Name
		title,                  // Window Title
		WS_CLIPSIBLINGS |           // Required Window Style
		WS_CLIPCHILDREN |           // Required Window Style
		dwStyle,                // Selected Window Style
		0, 0,                   // Window Position
		WindowRect.right - WindowRect.left,   // Calculate Adjusted Window Width
		WindowRect.bottom - WindowRect.top,   // Calculate Adjusted Window Height
		NULL,                   // No Parent Window
		NULL,                   // No Menu
		hInstance,              // Instance
		NULL)))                 // Don't Pass Anything To WM_CREATE
	{
		KillGLWindow();                         // Reset The Display
		MessageBox(NULL, L"Window Creation Error.", L"ERROR", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;                           // Return FALSE
	}

	static  PIXELFORMATDESCRIPTOR pfd =                  // pfd Tells Windows How We Want Things To Be
	{
		sizeof(PIXELFORMATDESCRIPTOR),                  // Size Of This Pixel Format Descriptor
		1,                              // Version Number
		PFD_DRAW_TO_WINDOW |                        // Format Must Support Window
		PFD_SUPPORT_OPENGL |                        // Format Must Support OpenGL
		PFD_DOUBLEBUFFER,                       // Must Support Double Buffering
		PFD_TYPE_RGBA,                          // Request An RGBA Format
		bits,                               // Select Our Color Depth
		0, 0, 0, 0, 0, 0,                       // Color Bits Ignored
		0,                              // No Alpha Buffer
		0,                              // Shift Bit Ignored
		0,                              // No Accumulation Buffer
		0, 0, 0, 0,                         // Accumulation Bits Ignored
		16,                             // 16Bit Z-Buffer (Depth Buffer)
		0,                              // No Stencil Buffer
		0,                              // No Auxiliary Buffer
		PFD_MAIN_PLANE,                         // Main Drawing Layer
		0,                              // Reserved
		0, 0, 0                             // Layer Masks Ignored
	};

	if (!(hDC = GetDC(hWnd)))                         // Did We Get A Device Context?
	{
		KillGLWindow();                         // Reset The Display
		MessageBox(NULL, L"Can't Create A GL Device Context.", L"ERROR", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;                           // Return FALSE
	}

	if (!(PixelFormat = ChoosePixelFormat(hDC, &pfd)))             // Did Windows Find A Matching Pixel Format?
	{
		KillGLWindow();                         // Reset The Display
		MessageBox(NULL, L"Can't Find A Suitable PixelFormat.", L"ERROR", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;                           // Return FALSE
	}

	if (!SetPixelFormat(hDC, PixelFormat, &pfd))               // Are We Able To Set The Pixel Format?
	{
		KillGLWindow();                         // Reset The Display
		return FALSE;                        // Return FALSE
	}

	if (!(hRC = wglCreateContext(hDC)))                   // Are We Able To Get A Rendering Context?
	{
		KillGLWindow();                         // Reset The Display
		MessageBox(NULL, L"Can't Create A GL Rendering Context.", L"ERROR", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;                           // Return FALSE
	}

	if (!wglMakeCurrent(hDC, hRC))                        // Try To Activate The Rendering Context
	{
		KillGLWindow();                         // Reset The Display
		MessageBox(NULL, L"Can't Activate The GL Rendering Context.", L"ERROR", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;                           // Return FALSE
	}

	ShowWindow(hWnd, SW_SHOW);                       // Show The Window
	SetForegroundWindow(hWnd);                      // Slightly Higher Priority
	SetFocus(hWnd);                             // Sets Keyboard Focus To The Window
	ReSizeGLScene(width, height);                       // Set Up Our Perspective GL Screen
	if (!InitGL())                              // Initialize Our Newly Created GL Window
	{
		KillGLWindow();                         // Reset The Display
		MessageBox(NULL, L"Initialization Failed.", L"ERROR", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;                           // Return FALSE
	}
	return TRUE;
}

LRESULT CALLBACK WndProc(HWND    hWnd,                   // Handle For This Window
	UINT    uMsg,                   // Message For This Window
	WPARAM  wParam,                 // Additional Message Information
	LPARAM  lParam)                 // Additional Message Information
{
	switch (uMsg)                               // Check For Windows Messages
	{
	case WM_ACTIVATE:                       // Watch For Window Activate Message
	{
		if (!HIWORD(wParam))                    // Check Minimization State
		{
			active = TRUE;                    // Program Is Active
		}
		else
		{
			active = FALSE;                   // Program Is No Longer Active
		}

		return 0;                       // Return To The Message Loop
	}

	case WM_SYSCOMMAND:                     // Intercept System Commands
	{
		switch (wParam)                     // Check System Calls
		{
		case SC_SCREENSAVE:             // Screensaver Trying To Start?
		case SC_MONITORPOWER:               // Monitor Trying To Enter Powersave?
			return 0;                   // Prevent From Happening
		}
		break;                          // Exit
	}

	case WM_CLOSE:                          // Did We Receive A Close Message?
	{
		PostQuitMessage(0);                 // Send A Quit Message
		return 0;                       // Jump Back
	}

	case WM_KEYDOWN:                        // Is A Key Being Held Down?
	{
		keys[wParam] = TRUE;                    // If So, Mark It As TRUE
		return 0;                       // Jump Back
	}

	case WM_KEYUP:                          // Has A Key Been Released?
	{
		keys[wParam] = FALSE;                   // If So, Mark It As FALSE
		return 0;                       // Jump Back
	}

	case WM_SIZE:                           // Resize The OpenGL Window
	{
		ReSizeGLScene(LOWORD(lParam), HIWORD(lParam));       // LoWord=Width, HiWord=Height
		return 0;                       // Jump Back
	}
	}
	// Pass All Unhandled Messages To DefWindowProc
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE   hInstance,              // Instance
	HINSTANCE   hPrevInstance,              // Previous Instance
	LPSTR       lpCmdLine,              // Command Line Parameters
	int     nCmdShow)               // Window Show State
{
	MSG msg;                                // Windows Message Structure
	BOOL    done = FALSE;

	// Ask The User Which Screen Mode They Prefer
	if (MessageBox(NULL, L"Would You Like To Run In Fullscreen Mode?", L"Start FullScreen?", MB_YESNO | MB_ICONQUESTION) == IDNO)
	{
		fullscreen = FALSE;                       // Windowed Mode
	}

	// Create Our OpenGL Window
	if (!CreateGLWindow(L"NeHe's OpenGL Framework", 640, 480, 16, fullscreen))
	{
		return 0;                           // Quit If Window Was Not Created
	}

	while (!done) {
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))           // Is There A Message Waiting?
		{
			if (msg.message == WM_QUIT)               // Have We Received A Quit Message?
			{
				done = TRUE;                  // If So done=TRUE
			}
			else                            // If Not, Deal With Window Messages
			{
				TranslateMessage(&msg);             // Translate The Message
				DispatchMessage(&msg);              // Dispatch The Message
			}
		}
		else                                // If There Are No Messages
		{
			// Draw The Scene.  Watch For ESC Key And Quit Messages From DrawGLScene()
			if (active)                     // Program Active?
			{
				if (keys[VK_ESCAPE])                // Was ESC Pressed?
				{
					done = TRUE;              // ESC Signalled A Quit
				}
				else                        // Not Time To Quit, Update Screen
				{
					DrawGLScene();              // Draw The Scene
					SwapBuffers(hDC);           // Swap Buffers (Double Buffering)

					if (keys[VK_LEFT])                          // Left Arrow Being Pressed?
					{
						yrot -= 0.2f;                         // If So Spin Cubes Left
					}
					if (keys[VK_RIGHT])                         // Right Arrow Being Pressed?
					{
						yrot += 0.2f;                         // If So Spin Cubes Right
					}
					if (keys[VK_UP])                            // Up Arrow Being Pressed?
					{
						xrot -= 0.2f;                         // If So Tilt Cubes Up
					}
					if (keys[VK_DOWN])                          // Down Arrow Being Pressed?
					{
						xrot += 0.2f;                         // If So Tilt Cubes Down
					}

					if (keys[VK_F1])                    // Is F1 Being Pressed?
					{
						keys[VK_F1] = FALSE;              // If So Make Key FALSE
						KillGLWindow();                 // Kill Our Current Window
						fullscreen = !fullscreen;             // Toggle Fullscreen / Windowed Mode
															  // Recreate Our OpenGL Window
						if (!CreateGLWindow(L"Nehe Texture & Lighting", 640, 480, 16, fullscreen))
						{
							return 0;               // Quit If Window Was Not Created
						}
					}
				}
			}
		}
	}
	// Shutdown
	KillGLWindow();                             // Kill The Window
	return (msg.wParam);                            // Exit The Program
}