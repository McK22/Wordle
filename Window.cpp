#include "Window.h"
#include <tchar.h>

void Window::moveCenter(int x, int y)
{
	RECT rect;
	rect.left = x - width / 2;
	rect.right = rect.left + width;
	rect.top = y - height / 2;
	rect.bottom = rect.top + height;

	AdjustWindowRect(&rect, GetWindowLong(hWnd, GWL_STYLE), GetMenu(hWnd) != NULL);
	SetWindowPos(hWnd, NULL, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, 0);
	UpdateWindow(hWnd);
}

void Window::resize(int width, int height)
{
	RECT rect;
	GetWindowRect(hWnd, &rect);
	int centerX = (rect.left + rect.right) / 2;
	int centerY = (rect.top + rect.bottom) / 2;

	SetWindowPos(hWnd, NULL, 0, 0, width, height, 0);
	moveCenter(centerX, centerY);
}

void Window::createBitmap()
{
	HDC hdc = GetDC(hWnd);
	if (offBitmap != NULL)
		DeleteObject(offBitmap);
	if (offOldBitmap != NULL)
		SelectObject(offDC, offOldBitmap);

	offBitmap = CreateCompatibleBitmap(hdc, width, height);
	offDC = CreateCompatibleDC(hdc);
	offOldBitmap = (HBITMAP)SelectObject(offDC, offBitmap);
	SetBkMode(offDC, TRANSPARENT);
	ReleaseDC(hWnd, hdc);

	HPEN pen = CreatePen(PS_NULL, 0, 0);
	SelectObject(offDC, pen);

	update();
}

void Window::update()
{
	InvalidateRect(hWnd, NULL, FALSE);
	UpdateWindow(hWnd);
}

bool Window::isFinished()
{
	return finished;
}

void Window::finish()
{
	finished = true;
}

int letterToInt(char c)
{
	return c - 'A';
}

void Window::drawBitmapContent()
{
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(hWnd, &ps);
	BitBlt(hdc, 0, 0, width, height, offDC, 0, 0, SRCCOPY);
	EndPaint(hWnd, &ps);
}

void Window::fillBitmapBackground()
{
	HBRUSH brush = CreateSolidBrush(COLOR_BACKGROUND);
	HBRUSH oldBrush = (HBRUSH)SelectObject(offDC, brush);
	Rectangle(offDC, -1, -1, width + 1, height + 1);
	SelectObject(offDC, oldBrush);
	DeleteObject(brush);
}

void Window::drawLetter(int x, int y, char c)
{
	TCHAR s[] = { c };
	HFONT font = createFont();
	HFONT oldFont = (HFONT)SelectObject(offDC, font);
	RECT rect;
	rect.left = x;
	rect.right = x + TILE_SIZE;
	rect.top = y;
	rect.bottom = y + TILE_SIZE;
	DrawText(offDC, s, 1, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	SelectObject(offDC, oldFont);
	DeleteObject(font);
}

HFONT Window::createFont()
{
	HFONT font = CreateFont(
		-MulDiv(12, GetDeviceCaps(offDC, LOGPIXELSY), 72),
		0,
		0,
		0,
		FW_BOLD,
		false,
		FALSE,
		0,
		EASTEUROPE_CHARSET,
		OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY,
		DEFAULT_PITCH | FF_SWISS,
		_T("Verdana"));
	return font;
}

void Window::drawRect(int x, int y, int width, int height, int color)
{
	if (color == COLOR_DEFAULT)
	{
		HPEN pen = CreatePen(PS_SOLID, PEN_THICKNESS, COLOR_BORDER);
		HPEN oldPen = (HPEN)SelectObject(offDC, pen);
		RoundRect(offDC, x, y, x + width, y + height, ROUND_SIZE, ROUND_SIZE);
		SelectObject(offDC, oldPen);
		DeleteObject(pen);
	}
	else
	{
		HBRUSH brush = CreateSolidBrush(color);
		HBRUSH oldBrush = (HBRUSH)SelectObject(offDC, brush);
		RoundRect(offDC, x, y, x + width, y + height, ROUND_SIZE, ROUND_SIZE);
		SelectObject(offDC, oldBrush);
		DeleteObject(brush);
	}
}
