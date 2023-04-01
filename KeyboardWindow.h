#pragma once

#include "GameWindow.h"

#define KEYBOARD_CLASS L"K"
#define TITLE L"WORDLE-KEYBOARD"
#define KEYBOARD_HEIGHT 3
#define MAX_KEYBOARD_WIDTH 10
#define NO_ANIMATION -1
#define ANIMATION_STEP 1000 / 60
#define BOTTOM_MARGIN 20
#define KEYBOARD_ALPHA 160

class KeyboardWindow : public Window
{
private:
	const int keyboardWidth[KEYBOARD_HEIGHT] = { 10, 9, 7 };
	const int letters[KEYBOARD_HEIGHT][MAX_KEYBOARD_WIDTH] = { { 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P' },
									{'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L'},
										{'Z', 'X', 'C', 'V', 'B', 'N', 'M' } };
	bool letterUsed[LETTER_COUNT] = { false };
	int screenWidth;
	int screenHeight;
	GameWindow* gameWindows[MAX_GAME_WINDOW_COUNT];
	int difficulty = IDM_EASY;
	std::string currentWord = "";
	int currentRow = 0;
	int animationTime[BOARD_HEIGHT_HARD];
	long long lastTimeMillis = 0;

public:
	KeyboardWindow(HINSTANCE hInst, int nCmdShow, int screenWidth, int screenHeight) : Window(hInst, nCmdShow),
		screenWidth(screenWidth), screenHeight(screenHeight)
	{
		width = (TILE_SIZE + MARGIN) * 10 + MARGIN;
		height = (TILE_SIZE + MARGIN) * 3 + MARGIN;
		for (int i = 0; i < MAX_GAME_WINDOW_COUNT; i++)
			gameWindows[i] = new GameWindow(hInst, nCmdShow, this);
		for (int i = 0; i < BOARD_HEIGHT_HARD; i++)
			animationTime[i] = NO_ANIMATION;
	};
	static ATOM registerClass(HINSTANCE hInst);
	virtual BOOL create();
	void createGameWindows();
	int getDifficulty() { return difficulty; }
	void paintGameWindow(HWND gameHwnd);
	int getBoardHeight();
	void destroyGameWindows();
	void setDifficulty(int difficulty);
	void type(char c);
	void erase();
	void setLetter(int x, int y, char c);
	void validateWord();
	void reset();
	void startGame();
	int getCurrentRow() { return currentRow; }
	void updateGameWindows();
	void finish();
	void paint();
	void drawTileBackground(int x, int y, char c);
	void startAnimation(int row);
	void updateAnimation();
	bool isRowInAnimation(int row);
	int getAnimationTime(int row);
};

LRESULT CALLBACK KeyboardWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
