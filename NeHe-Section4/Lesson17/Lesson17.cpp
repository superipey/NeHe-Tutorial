// NeHe01.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "Lesson17.h"
#include "GLWindow.h"

#include <Windows.h>
#include <gl\GL.h>
#include <gl\GLU.h>
#include <math.h>
#include <stdio.h>
#include <stdarg.h>			// Header File For Variable Argument Routines

#include "SOIL.h"
#include "GLWindow.h"

#pragma comment(lib,"opengl32.lib")
#pragma comment(lib,"glu32.lib")
#pragma comment(lib,"SOIL.lib")

HDC			hDC = NULL;		// Private GDI Device Context
HGLRC		hRC = NULL;		// Permanent Rendering Context
HWND		hWnd = NULL;		// Holds Our Window Handle
HINSTANCE	hInstance;		// Holds The Instance Of The Application

bool	keys[256];			// Array Used For The Keyboard Routine
bool	active = TRUE;		// Window Active Flag Set To TRUE By Default
bool	fullscreen = TRUE;	// Fullscreen Flag Set To Fullscreen Mode By Default

GLuint  base;                                   // Base Display List For The Font
GLuint  texture[2];                             // Storage For Our Font Texture
GLuint  loop;                                   // Generic Loop Variable

GLfloat cnt1;                                   // 1st Counter Used To Move Text & For Coloring
GLfloat cnt2;                                   // 2nd Counter Used To Move Text & For Coloring

LRESULT	CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);	// Declaration For WndProc

int LoadGLTextures()                                    // Load Bitmaps And Convert To Textures
{
	/* load an image file directly as a new OpenGL texture */
	texture[0] = SOIL_load_OGL_texture("Data/Font.bmp", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
	texture[1] = SOIL_load_OGL_texture("Data/Bumps.bmp", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
	
	if (texture[0] == 0 && texture[1] == 0) return false;

	// Typical Texture Generation Using Data From The Bitmap
	glBindTexture(GL_TEXTURE_2D, texture[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, texture[1]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	return TRUE;                                        // Return Success
}

GLvoid BuildFont(GLvoid)								// Build Our Font Display List
{
	float	cx;											// Holds Our X Character Coord
	float	cy;											// Holds Our Y Character Coord

	base = glGenLists(256);								// Creating 256 Display Lists
	glBindTexture(GL_TEXTURE_2D, texture[0]);			// Select Our Font Texture
	for (loop = 0; loop<256; loop++)						// Loop Through All 256 Lists
	{
		cx = float(loop % 16) / 16.0f;						// X Position Of Current Character
		cy = float(loop / 16) / 16.0f;						// Y Position Of Current Character

		glNewList(base + loop, GL_COMPILE);				// Start Building A List
		glBegin(GL_QUADS);							// Use A Quad For Each Character
		glTexCoord2f(cx, 1 - cy - 0.0625f);			// Texture Coord (Bottom Left)
		glVertex2i(0, 0);						// Vertex Coord (Bottom Left)
		glTexCoord2f(cx + 0.0625f, 1 - cy - 0.0625f);	// Texture Coord (Bottom Right)
		glVertex2i(16, 0);						// Vertex Coord (Bottom Right)
		glTexCoord2f(cx + 0.0625f, 1 - cy);			// Texture Coord (Top Right)
		glVertex2i(16, 16);						// Vertex Coord (Top Right)
		glTexCoord2f(cx, 1 - cy);					// Texture Coord (Top Left)
		glVertex2i(0, 16);						// Vertex Coord (Top Left)
		glEnd();									// Done Building Our Quad (Character)
		glTranslated(10, 0, 0);						// Move To The Right Of The Character
		glEndList();									// Done Building The Display List
	}													// Loop Until All 256 Are Built
}

GLvoid KillFont(GLvoid)									// Delete The Font From Memory
{
	glDeleteLists(base, 256);							// Delete All 256 Display Lists
}

GLvoid glPrint(GLint x, GLint y, char *string, int set)	// Where The Printing Happens
{
	if (set>1)
	{
		set = 1;
	}
	glBindTexture(GL_TEXTURE_2D, texture[0]);			// Select Our Font Texture
	glDisable(GL_DEPTH_TEST);							// Disables Depth Testing
	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glPushMatrix();										// Store The Projection Matrix
	glLoadIdentity();									// Reset The Projection Matrix
	glOrtho(0, 640, 0, 480, -1, 1);							// Set Up An Ortho Screen
	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
	glPushMatrix();										// Store The Modelview Matrix
	glLoadIdentity();									// Reset The Modelview Matrix
	glTranslated(x, y, 0);								// Position The Text (0,0 - Bottom Left)
	glListBase(base - 32 + (128 * set));						// Choose The Font Set (0 or 1)
	glCallLists(strlen(string), GL_UNSIGNED_BYTE, string);// Write The Text To The Screen
	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glPopMatrix();										// Restore The Old Projection Matrix
	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
	glPopMatrix();										// Restore The Old Projection Matrix
	glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
}

int InitGL(GLvoid)										// All Setup For OpenGL Goes Here
{
	if (!LoadGLTextures())								// Jump To Texture Loading Routine
	{
		return FALSE;									// If Texture Didn't Load Return FALSE
	}
	BuildFont();										// Build The Font
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);				// Clear The Background Color To Black
	glClearDepth(1.0);									// Enables Clearing Of The Depth Buffer
	glDepthFunc(GL_LEQUAL);								// The Type Of Depth Test To Do
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);					// Select The Type Of Blending
	glShadeModel(GL_SMOOTH);							// Enables Smooth Color Shading
	glEnable(GL_TEXTURE_2D);							// Enable 2D Texture Mapping
	return TRUE;											// Initialization Went OK
}

int DrawGLScene(GLvoid)                     // Here's Where We Do All The Drawing
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear The Screen And The Depth Buffer
	glLoadIdentity();									// Reset The Modelview Matrix
	glBindTexture(GL_TEXTURE_2D, texture[1]);			// Select Our Second Texture
	glTranslatef(0.0f, 0.0f, -5.0f);						// Move Into The Screen 5 Units
	glRotatef(45.0f, 0.0f, 0.0f, 1.0f);					// Rotate On The Z Axis 45 Degrees (Clockwise)
	glRotatef(cnt1*30.0f, 1.0f, 1.0f, 0.0f);				// Rotate On The X & Y Axis By cnt1 (Left To Right)
	glDisable(GL_BLEND);								// Disable Blending Before We Draw In 3D
	glColor3f(1.0f, 1.0f, 1.0f);							// Bright White
	glBegin(GL_QUADS);									// Draw Our First Texture Mapped Quad
	glTexCoord2d(0.0f, 0.0f);						// First Texture Coord
	glVertex2f(-1.0f, 1.0f);						// First Vertex
	glTexCoord2d(1.0f, 0.0f);						// Second Texture Coord
	glVertex2f(1.0f, 1.0f);						// Second Vertex
	glTexCoord2d(1.0f, 1.0f);						// Third Texture Coord
	glVertex2f(1.0f, -1.0f);						// Third Vertex
	glTexCoord2d(0.0f, 1.0f);						// Fourth Texture Coord
	glVertex2f(-1.0f, -1.0f);						// Fourth Vertex
	glEnd();											// Done Drawing The First Quad
	glRotatef(90.0f, 1.0f, 1.0f, 0.0f);					// Rotate On The X & Y Axis By 90 Degrees (Left To Right)
	glBegin(GL_QUADS);									// Draw Our Second Texture Mapped Quad
	glTexCoord2d(0.0f, 0.0f);						// First Texture Coord
	glVertex2f(-1.0f, 1.0f);						// First Vertex
	glTexCoord2d(1.0f, 0.0f);						// Second Texture Coord
	glVertex2f(1.0f, 1.0f);						// Second Vertex
	glTexCoord2d(1.0f, 1.0f);						// Third Texture Coord
	glVertex2f(1.0f, -1.0f);						// Third Vertex
	glTexCoord2d(0.0f, 1.0f);						// Fourth Texture Coord
	glVertex2f(-1.0f, -1.0f);						// Fourth Vertex
	glEnd();											// Done Drawing Our Second Quad
	glEnable(GL_BLEND);									// Enable Blending

	glLoadIdentity();									// Reset The View
														// Pulsing Colors Based On Text Position
	glColor3f(1.0f*float(cos(cnt1)), 1.0f*float(sin(cnt2)), 1.0f - 0.5f*float(cos(cnt1 + cnt2)));
	glPrint(int((280 + 250 * cos(cnt1))), int(235 + 200 * sin(cnt2)), "NeHe", 0);		// Print GL Text To The Screen

	glColor3f(1.0f*float(sin(cnt2)), 1.0f - 0.5f*float(cos(cnt1 + cnt2)), 1.0f*float(cos(cnt1)));
	glPrint(int((280 + 230 * cos(cnt2))), int(235 + 200 * sin(cnt1)), "OpenGL", 1);	// Print GL Text To The Screen

	glColor3f(0.0f, 0.0f, 1.0f);							// Set Color To Blue
	glPrint(int(240 + 200 * cos((cnt2 + cnt1) / 5)), 2, "Giuseppe D'Agata", 0);

	glColor3f(1.0f, 1.0f, 1.0f);							// Set Color To White
	glPrint(int(242 + 200 * cos((cnt2 + cnt1) / 5)), 2, "Giuseppe D'Agata", 0);

	cnt1 += 0.01f;										// Increase The First Counter
	cnt2 += 0.0081f;										// Increase The Second Counter
	return TRUE;										// Everything Went OK
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

	KillFont();
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