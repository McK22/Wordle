#pragma once
#include "framework.h"
#include "resource.h"
#include "wordle.h"
#include <string>

#define TILE_SIZE 55
#define MARGIN 6
#define BOARD_WIDTH 5
#define BOARD_HEIGHT_EASY 6
#define BOARD_HEIGHT_MEDIUM 8
#define BOARD_HEIGHT_HARD 10
#define COLOR_BACKGROUND RGB(255, 255, 255)
#define COLOR_DEFAULT RGB(251,252,255)
#define COLOR_WRONGLETTER RGB(164, 174, 196)
#define COLOR_WRONGSPOT RGB(243,194,55)
#define COLOR_CORRECT RGB(121,184,81)
#define COLOR_BORDER RGB(230, 230, 230)
#define ROUND_SIZE 4
#define MAX_GAME_WINDOW_COUNT 4
#define STATUS_DEFAULT 0
#define STATUS_WRONGLETTER 1
#define STATUS_WRONGSPOT 2
#define STATUS_CORRECT 3
#define LETTER_COUNT 26
#define PEN_THICKNESS 2
#define TILE_ANIMATION_DURATION 300
#define IDT_TIMER 1

class Window
{
protected:
	HINSTANCE hInst;
	int nCmdShow;
	HWND hWnd = nullptr;
	int width = 0;
	int height = 0;
	HDC offDC = NULL;
	HBITMAP offOldBitmap = NULL;
	HBITMAP offBitmap = NULL;
	bool finished = false;
	HFONT createFont();
	void drawLetter(int x, int y, char c);

public:
	Window(HINSTANCE hInst, int nCmdShow) : hInst(hInst), nCmdShow(nCmdShow) {};
	virtual BOOL create() = 0;
	void moveCenter(int x, int y);
	void resize(int width, int height);
	HWND getHwnd() { return hWnd; }
	void createBitmap();
	void update();
	bool isFinished();
	void finish();
	void fillBitmapBackground();
	void drawBitmapContent();
	void drawRect(int x, int y, int width, int height, int color);
};

int letterToInt(char c);