#pragma once

#include "Window.h"

#define GAME_CLASS L"G"
#define TITLE L"WORDLE-KEYBOARD"
#define OVERLAY_ALPHA 150
#define COLOR_WIN RGB(0, 255, 0)
#define COLOR_LOSE RGB(255, 0, 0)
#define COLOR_WHITE RGB(255, 255, 255)

class KeyboardWindow;

class GameWindow : public Window
{
private:
	KeyboardWindow* keyboardWindow;
	std::string wordToGuess;
	char letters[BOARD_HEIGHT_HARD][BOARD_WIDTH] = { {'\0'} };
	int letterColorOnBoard[BOARD_HEIGHT_HARD][BOARD_WIDTH];
	int letterColor[LETTER_COUNT];
	int completedRows = 0;
	int lastRow;

public:
	GameWindow(HINSTANCE hInst, int nCmdShow, KeyboardWindow* keyboardWindow) : Window(hInst, nCmdShow), keyboardWindow(keyboardWindow)
	{
		for (int i = 0; i < BOARD_HEIGHT_HARD; i++)
			for (int j = 0; j < BOARD_WIDTH; j++)
				letterColorOnBoard[i][j] = COLOR_DEFAULT;

		for (int i = 0; i < LETTER_COUNT; i++)
			letterColor[i] = COLOR_DEFAULT;
	}
	static ATOM registerClass(HINSTANCE hInst);
	virtual BOOL create();
	void destroy();
	void setLetter(int x, int y, char c);
	void reset();
	void paint();
	void drawConfirmedLetters(int& x, int& y);
	void drawUnconfirmedLetters(int& x, int& y);
	void confirmWord(int row);
	void drawFinish();
	bool isWordCorrect();
	void setWordToGuess(std::string s);
	void finish();
	int getLetterColor(char c);
	void drawCorrectWord();
	void drawAnimatedRow(int row);
	void drawTile(int row, int column, int color);
	int getTileX(int column);
	int getTileY(int row);
};

LRESULT CALLBACK GameWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
