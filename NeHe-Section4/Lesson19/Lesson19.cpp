// Lesson06.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "Lesson19.h"
#include <gl\GL.h>
#include <gl\GLU.h>
#include <stdio.h>
#include "SOIL.h"
#include "GLWindow.h"

#pragma comment(lib,"opengl32.lib")
#pragma comment(lib,"glu32.lib")
#pragma comment(lib,"SOIL.lib")

#define MAX_PARTICLES   1000        // Number Of Particles To Create ( NEW )

HDC     hDC = NULL;       // Private GDI Device Context
HGLRC       hRC = NULL;       // Permanent Rendering Context
HWND        hWnd = NULL;      // Holds Our Window Handle
HINSTANCE   hInstance;      // Holds The Instance Of The Application

bool    keys[256];          // Array Used For The Keyboard Routine
bool    active = TRUE;            // Window Active Flag Set To TRUE By Default
bool    fullscreen = TRUE;        // Fullscreen Flag Set To Fullscreen Mode By Default
bool    rainbow = true;           // Rainbow Mode?    ( ADD )
bool    sp;             // Spacebar Pressed?    ( ADD )
bool    rp;             // Return Key Pressed?  ( ADD )

float   slowdown = 2.0f;          // Slow Down Particles
float   xspeed;             // Base X Speed (To Allow Keyboard Direction Of Tail)
float   yspeed;             // Base Y Speed (To Allow Keyboard Direction Of Tail)
float   zoom = -40.0f;            // Used To Zoom Out

GLuint  loop;               // Misc Loop Variable
GLuint  col;                // Current Color Selection
GLuint  delay;              // Rainbow Effect Delay
GLuint  texture[1];         // Storage For Our Particle Texture

typedef struct                      // Create A Structure For Particle
{
	bool    active;                 // Active (Yes/No)
	float   life;                   // Particle Life
	float   fade;                   // Fade Speed
	float   r;                  // Red Value
	float   g;                  // Green Value
	float   b;                  // Blue Value
	float   x;                  // X Position
	float   y;                  // Y Position
	float   z;                  // Z Position
	float   xi;                 // X Direction
	float   yi;                 // Y Direction
	float   zi;                 // Z Direction
	float   xg;                 // X Gravity
	float   yg;                 // Y Gravity
	float   zg;                 // Z Gravity
}
particles;                      // Particles Structure

particles particle[MAX_PARTICLES];          // Particle Array (Room For Particle Info)

static GLfloat colors[12][3] =               // Rainbow Of Colors
{
	{ 1.0f,0.5f,0.5f },{ 1.0f,0.75f,0.5f },{ 1.0f,1.0f,0.5f },{ 0.75f,1.0f,0.5f },
	{ 0.5f,1.0f,0.5f },{ 0.5f,1.0f,0.75f },{ 0.5f,1.0f,1.0f },{ 0.5f,0.75f,1.0f },
	{ 0.5f,0.5f,1.0f },{ 0.75f,0.5f,1.0f },{ 1.0f,0.5f,1.0f },{ 1.0f,0.5f,0.75f }
};

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);   // Declaration For WndProc

int LoadGLTextures()                                    // Load Bitmaps And Convert To Textures
{
	/* load an image file directly as a new OpenGL texture */
	texture[0] = SOIL_load_OGL_texture
	(
		"Data/Particle.bmp",
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_INVERT_Y
	);

	if (texture[0] == 0) return false;

	// Typical Texture Generation Using Data From The Bitmap
	glBindTexture(GL_TEXTURE_2D, texture[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	return TRUE;                                        // Return Success
}

int InitGL(GLvoid)                              // All Setup For OpenGL Goes Here
{
	if (!LoadGLTextures())                          // Jump To Texture Loading Routine ( NEW )
	{
		return FALSE;                           // If Texture Didn't Load Return FALSE ( NEW )
	}

	glShadeModel(GL_SMOOTH);                        // Enables Smooth Shading
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);                  // Black Background
	glClearDepth(1.0f);                         // Depth Buffer Setup
	glDisable(GL_DEPTH_TEST);                       // Disables Depth Testing
	glEnable(GL_BLEND);                         // Enable Blending
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);                   // Type Of Blending To Perform
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);           // Really Nice Perspective Calculations
	glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);                 // Really Nice Point Smoothing
	glEnable(GL_TEXTURE_2D);                        // Enable Texture Mapping
	glBindTexture(GL_TEXTURE_2D, texture[0]);                // Select Our Texture

	for (loop = 0; loop<MAX_PARTICLES; loop++)                   // Initialize All The Textures
	{
		particle[loop].active = true;                 // Make All The Particles Active
		particle[loop].life = 1.0f;                   // Give All The Particles Full Life	
		particle[loop].fade = float(rand() % 100) / 1000.0f + 0.003f;       // Random Fade Speed
		particle[loop].r = colors[loop*(12 / MAX_PARTICLES)][0];        // Select Red Rainbow Color
		particle[loop].g = colors[loop*(12 / MAX_PARTICLES)][1];        // Select Red Rainbow Color
		particle[loop].b = colors[loop*(12 / MAX_PARTICLES)][2];        // Select Red Rainbow Color
		particle[loop].xi = float((rand() % 50) - 26.0f)*10.0f;       // Random Speed On X Axis
		particle[loop].yi = float((rand() % 50) - 25.0f)*10.0f;       // Random Speed On Y Axis
		particle[loop].zi = float((rand() % 50) - 25.0f)*10.0f;       // Random Speed On Z Axis
		particle[loop].xg = 0.0f;                     // Set Horizontal Pull To Zero
		particle[loop].yg = -0.8f;                    // Set Vertical Pull Downward
		particle[loop].zg = 0.0f;                     // Set Pull On Z Axis To Zero
	}

	return TRUE;                                // Initialization Went OK
}

int DrawGLScene(GLvoid)                             // Here's Where We Do All The Drawing
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);         // Clear Screen And Depth Buffer
	glLoadIdentity();                           // Reset The ModelView Matrix

	for (loop = 0; loop < MAX_PARTICLES; loop++)                   // Loop Through All The Particles
	{
		if (particle[loop].active)                  // If The Particle Is Active
		{
			float x = particle[loop].x;               // Grab Our Particle X Position
			float y = particle[loop].y;               // Grab Our Particle Y Position
			float z = particle[loop].z + zoom;              // Particle Z Pos + Zoom

															// Draw The Particle Using Our RGB Values, Fade The Particle Based On It's Life
			glColor4f(particle[loop].r, particle[loop].g, particle[loop].b, particle[loop].life);
			glBegin(GL_TRIANGLE_STRIP);             // Build Quad From A Triangle Strip
			glTexCoord2d(1, 1); glVertex3f(x + 0.5f, y + 0.5f, z); // Top Right
			glTexCoord2d(0, 1); glVertex3f(x - 0.5f, y + 0.5f, z); // Top Left
			glTexCoord2d(1, 0); glVertex3f(x + 0.5f, y - 0.5f, z); // Bottom Right
			glTexCoord2d(0, 0); glVertex3f(x - 0.5f, y - 0.5f, z); // Bottom Left

			glEnd();                        // Done Building Triangle Strip
			particle[loop].x += particle[loop].xi / (slowdown * 1000);    // Move On The X Axis By X Speed
			particle[loop].y += particle[loop].yi / (slowdown * 1000);    // Move On The Y Axis By Y Speed
			particle[loop].z += particle[loop].zi / (slowdown * 1000);    // Move On The Z Axis By Z Speed
			particle[loop].xi += particle[loop].xg;           // Take Pull On X Axis Into Account
			particle[loop].yi += particle[loop].yg;           // Take Pull On Y Axis Into Account
			particle[loop].zi += particle[loop].zg;           // Take Pull On Z Axis Into Account
			particle[loop].life -= particle[loop].fade;       // Reduce Particles Life By 'Fade'
			if (particle[loop].life < 0.0f)                    // If Particle Is Burned Out
			{
				particle[loop].life = 1.0f;               // Give It New Life
				particle[loop].fade = float(rand() % 100) / 1000.0f + 0.003f;   // Random Fade Value
				particle[loop].x = 0.0f;                  // Center On X Axis
				particle[loop].y = 0.0f;                  // Center On Y Axis
				particle[loop].z = 0.0f;                  // Center On Z Axis
				particle[loop].xi = xspeed + float((rand() % 60) - 32.0f);  // X Axis Speed And Direction
				particle[loop].yi = yspeed + float((rand() % 60) - 30.0f);  // Y Axis Speed And Direction
				particle[loop].zi = float((rand() % 60) - 30.0f);     // Z Axis Speed And Direction
				particle[loop].r = colors[col][0];            // Select Red From Color Table
				particle[loop].g = colors[col][1];            // Select Green From Color Table
				particle[loop].b = colors[col][2];            // Select Blue From Color Table
			}
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
	gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 200.0f);

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
	if (!CreateGLWindow(L"Lesson 19", 640, 480, 16, fullscreen))
	{
		return 0;                           // Quit If Window Was Not Created
	}

	if (fullscreen)                         // Are We In Fullscreen Mode ( ADD )
	{
		slowdown = 1.0f;                      // Speed Up The Particles (3dfx Issue) ( ADD )
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
				}
			}

			// If Number Pad 8 And Y Gravity Is Less Than 1.5 Increase Pull Upwards
			if (keys[VK_NUMPAD8] && (particle[loop].yg<1.5f)) particle[loop].yg += 0.01f;

			// If Number Pad 2 And Y Gravity Is Greater Than -1.5 Increase Pull Downwards
			if (keys[VK_NUMPAD2] && (particle[loop].yg>-1.5f)) particle[loop].yg -= 0.01f;

			// If Number Pad 6 And X Gravity Is Less Than 1.5 Increase Pull Right
			if (keys[VK_NUMPAD6] && (particle[loop].xg<1.5f)) particle[loop].xg += 0.01f;

			// If Number Pad 4 And X Gravity Is Greater Than -1.5 Increase Pull Left
			if (keys[VK_NUMPAD4] && (particle[loop].xg>-1.5f)) particle[loop].xg -= 0.01f;

			if (keys[VK_ADD] && (slowdown>1.0f)) slowdown -= 0.01f;        // Speed Up Particles

			if (keys[VK_SUBTRACT] && (slowdown<4.0f)) slowdown += 0.01f;   // Slow Down Particles

			if (keys[VK_PRIOR]) zoom += 0.1f;     // Zoom In

			if (keys[VK_NEXT]) zoom -= 0.1f;      // Zoom Out

			if (keys[VK_RETURN] && !rp)     // Return Key Pressed
			{
				rp = true;            // Set Flag Telling Us It's Pressed
				rainbow = !rainbow;       // Toggle Rainbow Mode On / Off
			}
			if (!keys[VK_RETURN]) rp = false;     // If Return Is Released Clear Flag
			
			if ((keys[' '] && !sp) || (rainbow && (delay>25)))   // Space Or Rainbow Mode
			{
				if (keys[' ']) rainbow = false;   // If Spacebar Is Pressed Disable Rainbow Mode
				sp = true;            // Set Flag Telling Us Space Is Pressed
				delay = 0;            // Reset The Rainbow Color Cycling Delay
				col++;
				if (col>11) col = 0;       // If Color Is To High Reset It
			}

			if (!keys[' ']) sp = false;       // If Spacebar Is Released Clear Flag

			// If Up And Y Speed Is Less Than 200 Increase Upward Speed
			if (keys[VK_UP] && (yspeed<200)) yspeed += 1.0f;
			
			// If Down Arrow And Y Speed Is Greater Than -200 Increase Downward Speed
			if (keys[VK_DOWN] && (yspeed>-200)) yspeed -= 1.0f;

			// If Right Arrow And X Speed Is Less Than 200 Increase Speed To The Right
			if (keys[VK_RIGHT] && (xspeed<200)) xspeed += 1.0f;


			// If Left Arrow And X Speed Is Greater Than -200 Increase Speed To The Left
			if (keys[VK_LEFT] && (xspeed>-200)) xspeed -= 1.0f;

			delay++;            // Increase Rainbow Mode Color Cycling Delay Counter

			if (keys[VK_TAB])                       // Tab Key Causes A Burst
			{
				particle[loop].x = 0.0f;                  // Center On X Axis
				particle[loop].y = 0.0f;                  // Center On Y Axis
				particle[loop].z = 0.0f;                  // Center On Z Axis
				particle[loop].xi = float((rand() % 50) - 26.0f)*10.0f;   // Random Speed On X Axis
				particle[loop].yi = float((rand() % 50) - 25.0f)*10.0f;   // Random Speed On Y Axis
				particle[loop].zi = float((rand() % 50) - 25.0f)*10.0f;   // Random Speed On Z Axis
			}


			if (keys[VK_F1])                    // Is F1 Being Pressed?
			{
				keys[VK_F1] = FALSE;              // If So Make Key FALSE
				KillGLWindow();                 // Kill Our Current Window
				fullscreen = !fullscreen;             // Toggle Fullscreen / Windowed Mode
													  // Recreate Our OpenGL Window
				if (!CreateGLWindow(L"Lesson 19", 640, 480, 16, fullscreen))
				{
					return 0;               // Quit If Window Was Not Created
				}
			}
		}
	}
	// Shutdown
	KillGLWindow();                             // Kill The Window
	return (msg.wParam);                            // Exit The Program
}