#include "main.h"

// the entry point for any Windows program
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    // this struct holds information for the window class
    WNDCLASSEX wc;
    ZeroMemory(&wc, sizeof(WNDCLASSEX));

    // fill in the struct with the needed information
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.lpszClassName = L"PicturePuzzle";

    // register the window class
    RegisterClassEx(&wc);

    // create the window and use the result as the handle
    HWND hWnd = CreateWindowEx(NULL,
                          L"PicturePuzzle",    // name of the window class
                          L"Picture Puzzle (test task)",   // title of the window
                          WS_OVERLAPPEDWINDOW,    // window style
                          300,    // x-position of the window
                          300,    // y-position of the window
                          SCREEN_WIDTH, SCREEN_HEIGHT,    // set window to new resolution
                          NULL,    // we have no parent window, NULL
                          NULL,    // we aren't using menus, NULL
                          hInstance,    // application handle
                          NULL);    // used with multiple windows, NULL
    // display the window on the screen
    ShowWindow(hWnd, nCmdShow);

	PuzzleGame = new MiniGamePicturePuzzle;
	if(!PuzzleGame)
		return 0;

	// Initialize and run the system object.
	PuzzleGame->hWnd = hWnd;
	PuzzleGame->Initialize();

    // this struct holds Windows event messages
    MSG msg = {0};

    // enter the main loop:
	while(TRUE)
    {
        if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);

            if(msg.message == WM_QUIT)
                break;
        }
        PuzzleGame->Render();
    }
	// Delete object on program quit
	delete PuzzleGame;
	PuzzleGame = NULL;
	return 0;
}

// this is the main message handler for the program
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	std::wstringstream WStrStream;
    // sort through and find what code to run for the message given
    switch(message)
    {
        // this message is read when the window is closed
        case WM_DESTROY:
            {
                // close the application entirely
                PostQuitMessage(0);
                return 0;
            } break;
			
		case WM_LBUTTONDOWN: //on mouse click
				PuzzleGame->Click((float)GET_X_LPARAM(lParam), (float)GET_Y_LPARAM(lParam));
			break;
    }

    // Handle any messages the switch statement didn't
    return DefWindowProc (hWnd, message, wParam, lParam);
}