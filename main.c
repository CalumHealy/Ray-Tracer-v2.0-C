#include <windows.h>
#include <gl/gl.h> // OpenGL header
#include <stdio.h>
#include <stdlib.h> // For malloc and free

HGLRC hglrc;

// Window Procedure function to handle messages sent to the window
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
		case WM_SIZE: {
			int width = LOWORD(lParam);
			int height = HIWORD(lParam);
			glViewport(0, 0, width, height);	// Update the viewport
			break;
		}
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

GLuint texture;
GLubyte *pixels = NULL;
int currentWidth = 0, currentHeight = 0;

// Pre-allocate a buffer for pixels (consider a larger size for larger textures)
void AllocatePixelsBuffer(int width, int height) {
	if (pixels != NULL) {
		free(pixels);
	}
	pixels = (GLubyte *)malloc(width * height * 3 * sizeof(GLubyte));
	if (pixels == NULL) {
		printf("Memory allocation for pixels failed.\n");
		return;
	}
	currentWidth = width;
	currentHeight = height;
}

void InitializeOpenGL(HWND hwnd) {
	// Get device context
	HDC hdc = GetDC(hwnd);

	// Set the pixel format for the DC
	PIXELFORMATDESCRIPTOR pfd = {
		sizeof(PIXELFORMATDESCRIPTOR),
		1,
		PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
		PFD_TYPE_RGBA,
		32,				// Colour depth
		0, 0, 0, 0, 0, 0,		// Colour bits ignored
		0,				// No alpha buffer
		0,				// Shift bits ignored
		0,				// No accumulation buffer
		0, 0, 0, 0,			// Accum bits ignored
		24,				// 24-bit z-buffer
		8,				// 8-bit stencil buffer
		0,				// No auxiliary buffer
		PFD_MAIN_PLANE,
		0,				// Reserved
		0, 0, 0				// Layer masks ignored
	};
	int format = ChoosePixelFormat(hdc, &pfd);
	SetPixelFormat(hdc, format, &pfd);

	// Create and enable the OpenGL rendering context
	// HGLRC hglrc = wglCreateContext(hdc);
	hglrc = wglCreateContext(hdc);
	if (!hglrc) {
		printf("OpenGL context creation failed.\n");
		return;
	}
	wglMakeCurrent(hdc, hglrc);

	// Set up OpenGL for 2D rendering
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, 800, 600, 0, -1, 1);	// Orthographic projection based on window size

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Create the texture once and set parameters
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// Release device context
	ReleaseDC(hwnd, hdc);

	GLenum err;
	while ((err = glGetError()) != GL_NO_ERROR) {
		printf("OpenGL error: %d\n", err);	// Replace with a logger or breakpoint in production
	}
}

// GLubyte* pixels = NULL;
// int currentWidth = 0, currentHeight = 0;

// void AllocatePixelsBuffer(int width, int height) {
// 	if (pixels != NULL) {
// 		free(pixels);
// 	}
// 	pixels = (GLubyte*)malloc(width * height * 3 * sizeof(GLubyte));
// 	if (pixels == NULL) {
// 		printf("Memory allocation for pixels failed.\n");
// 		return;
// 	}
// 	currentWidth = width;
// 	currentHeight = height;
// }

void RenderScene(int width, int height) {
	if (width != currentWidth || height != currentHeight) {
		AllocatePixelsBuffer(width, height);
	}

	// Clear the window with a black background
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);

	// Example of setting pixels in a 2D texture
	// const int width = 800;
	// const int height = 600;
	// GLubyte pixels[width * height * 3]; // RGB array
	// GLubyte* pixels = new GLubyte[width * height * 3];	// C++ code
	// GLubyte* pixels = (GLubyte*)malloc(width * height * 3 * sizeof(GLubyte));

	// Set up pixel colours
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			int index = (y * width + x) *3;
			pixels[index] = (GLubyte)(x % 255);		// R
			pixels[index + 1] = (GLubyte)(y % 255);		// G
			pixels[index + 2] = (GLubyte)(128);		// B
		}
	}

	// Create and bind the texture
	// GLuint texture;
	// glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, (GLvoid*)pixels);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, (GLvoid*)pixels);

	// delete[] pixels;	C++ code
	free(pixels);

	// Enable texturing and render the texture as a quad
	glEnable(GL_TEXTURE_2D);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f); glVertex2i(0, 0);
	glTexCoord2f(1.0f, 0.0f); glVertex2i(width, 0);
	glTexCoord2f(1.0f, 1.0f); glVertex2i(width, height);
	glTexCoord2f(0.0f, 1.0f); glVertex2i(0, height);
	glEnd();

	glDisable(GL_TEXTURE_2D);
	// glDeleteTextures(1, &texture);

	GLenum err;
	while ((err = glGetError()) != GL_NO_ERROR) {
		printf("OpenGL error: %d\n", err);	// Replace with a logger or breakpoint in production
	}

	free(pixels);
	pixels = NULL;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	// Step 1: Register the Window Class
	const char CLASS_NAME[] = "Sample Window Class";

	// WNDCLASS wc = {0}; C++ code
	// wc.lpfnWndProc = WindowProc;
	// wc.hInstance = hInstance;
	// wc.lpszClassName = CLASS_NAME;

	WNDCLASS wc;
	wc.style = 0;
	wc.lpfnWndProc = WindowProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = CLASS_NAME;

	// RegisterClass(&wc);
	if (!RegisterClass(&wc)) {
		return -1;			// Return an error code if registration fails
	}

	// Step 2: Create the Window
	HWND hwnd = CreateWindowEx(
		0,				// Optional window styles
		CLASS_NAME,			// Window class
		"Hello, Windows!",		// Window text
		WS_OVERLAPPEDWINDOW,		// Window style

		// Position and size
		// CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		// 500, 400, 800, 600,
		CW_USEDEFAULT, CW_USEDEFAULT, 1400, 1000,

		NULL,				// Parent window
		NULL, 				// Menu
		hInstance,			// Instance handle
		NULL				// Additional application data
	);

	// if (hwnd == NULL) {
	//	return 0;
	// }
	if (hwnd == NULL) {
		return -1;			// Return an error code if window creation fails
	}

	ShowWindow(hwnd, nCmdShow);

	InitializeOpenGL(hwnd);

	MSG msg;
	while (1) {
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT)
				break;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		} else {
			RECT rect;
			GetClientRect(hwnd, &rect);
			int width = rect.right - rect.left;
			int height = rect.bottom - rect.top;

			RenderScene(width, height);

			// Swap buffers to display the rendered image
			HDC hdc = GetDC(hwnd);
			SwapBuffers(hdc);
			ReleaseDC(hwnd, hdc);
		}
	}

	// Step 3: Run the Message Loop
	// MSG msg = {0};
	// MSG msg;
	// while (GetMessage(&msg, NULL, 0, 0)) {
	//	TranslateMessage(&msg);
	//	DispatchMessage(&msg);
	// }

	// if (pixels != NULL) {
	// 	free(pixels);
	// }

	// Cleanup OpenGL context
	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(hglrc);

	if (pixels != NULL) {
		free(pixels);
	}
	glDeleteTextures(1, &texture);

	return 0;
}