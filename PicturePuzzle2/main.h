#include <windows.h>
#include <windowsx.h>

#include "minigame_picturepuzzle.h"

MiniGamePicturePuzzle* PuzzleGame; //global var

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
