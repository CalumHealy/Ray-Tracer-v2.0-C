#include <windows.h>

// Window Procedure function to handle messages sent to the window
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	// Step 1: Register the Window Class
	const char CLASS_NAME[] = "Sample Window Class";

	WNDCLASS wc = {0};
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = CLASS_NAME;

	RegisterClass(&wc);

	// Step 2: Create the Window
	HWND hwnd = CreateWindowEx(
		0,				// Optional window styles
		CLASS_NAME,			// Window class
		"Hello, Windows!",		// Window text
		WS_OVERLAPPEDWINDOW,		// Window style

		// Position and size
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

		NULL,				// Parent window
		NULL, 				// Menu
		hInstance,			// Instance handle
		NULL				// Additional application data
	);

	if (hwnd == NULL) {
		return 0;
	}

	ShowWindow(hwnd, nCmdShow);

	// Step 3: Run the Message Loop
	MSG msg = {0};
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}