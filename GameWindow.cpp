#include "GameWindow.h"
#include "KeyboardWindow.h"
#include <tchar.h>

void GameWindow::paint()
{
    fillBitmapBackground();

    int x = MARGIN;
    int y = MARGIN;

    for (int i = 0; i < keyboardWindow->getBoardHeight(); i++)
    {
        if (keyboardWindow->isRowInAnimation(i))
            drawAnimatedRow(i);
        else
            for (int j = 0; j < BOARD_WIDTH; j++)
                drawTile(i, j, letterColorOnBoard[i][j]);
    }

    if (isFinished() && !keyboardWindow->isRowInAnimation(lastRow))
        drawFinish();

    drawBitmapContent();
}

bool GameWindow::isWordCorrect()
{
    int currentRow;
    if (finished)
        currentRow = completedRows - 1;
    else
        currentRow = keyboardWindow->getCurrentRow() - 1;
    for (int i = 0; i < BOARD_WIDTH; i++)
        if (tolower(letters[currentRow][i]) != tolower(wordToGuess[i]))
            return false;
    return true;
}

void GameWindow::drawCorrectWord()
{
    COLORREF oldColor = SetTextColor(offDC, COLOR_WHITE);
    TCHAR s[BOARD_WIDTH];
    for (int i = 0; i < BOARD_WIDTH; i++)
        s[i] = toupper(wordToGuess[i]);
    HFONT font = createFont();
    HFONT oldFont = (HFONT)SelectObject(offDC, font);
    RECT rect;
    rect.left = 0;
    rect.right = width;
    rect.top = 0;
    rect.bottom = height;
    DrawText(offDC, s, BOARD_WIDTH, &rect,  DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    SelectObject(offDC, oldFont);
    DeleteObject(font);
    SetTextColor(offDC, oldColor);
}

void GameWindow::drawFinish()
{
    HDC overlayDC = CreateCompatibleDC(offDC);
    HBITMAP bitmap = CreateCompatibleBitmap(offDC, width, height);
    HPEN pen = CreatePen(PS_NULL, 0, 0);
    HBRUSH brush;
    bool win = isWordCorrect();
    if (win)
        brush = CreateSolidBrush(COLOR_WIN);
    else
        brush = CreateSolidBrush(COLOR_LOSE);
    SelectObject(overlayDC, bitmap);
    SelectObject(overlayDC, brush);
    SelectObject(overlayDC, pen);
    Rectangle(overlayDC, 0, 0, width, height);

    BLENDFUNCTION bf = { 0 };
    bf.BlendOp = AC_SRC_OVER;
    bf.BlendFlags = 0;
    bf.SourceConstantAlpha = OVERLAY_ALPHA;
    bf.AlphaFormat = 0;
    BOOL a = AlphaBlend(offDC, 0, 0, width, height, overlayDC, 0, 0, width, height, bf);

    DeleteObject(pen);
    DeleteObject(brush);
    DeleteObject(bitmap);
    DeleteDC(overlayDC);

    if (!win)
        drawCorrectWord();
}

void GameWindow::setLetter(int x, int y, char c)
{
    letters[x][y] = c;
}

void GameWindow::reset()
{
    for (int i = 0; i < BOARD_HEIGHT_HARD; i++)
    {
        for (int j = 0; j < BOARD_WIDTH; j++)
        {
            letters[i][j] = '\0';
            letterColorOnBoard[i][j] = COLOR_DEFAULT;
        }
    }

    for (int i = 0; i < LETTER_COUNT; i++)
        letterColor[i] = COLOR_DEFAULT;
}

BOOL GameWindow::create()
{
    finished = false;
    completedRows = 0;

    hWnd = CreateWindow(GAME_CLASS, L"WORDLE-PUZZLE", 0,
        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, keyboardWindow->getHwnd(), nullptr, hInst, nullptr);

    if (!hWnd)
        return false;

    int rows;
    int difficulty = keyboardWindow->getDifficulty();
    if (difficulty == IDM_EASY)
        rows = BOARD_HEIGHT_EASY;
    else if (difficulty == IDM_MEDIUM)
        rows = BOARD_HEIGHT_MEDIUM;
    else
        rows = BOARD_HEIGHT_HARD;

    width = (TILE_SIZE + MARGIN) * BOARD_WIDTH + MARGIN;
    height = (TILE_SIZE + MARGIN) * rows + MARGIN;

    ShowWindow(hWnd, nCmdShow);
    createBitmap();

    return true;
}

void GameWindow::setWordToGuess(std::string s)
{
    wordToGuess = s;
}

void GameWindow::confirmWord(int row)
{
    for (int i = 0; i < BOARD_WIDTH; i++)
    {
        if (tolower(letters[row][i]) == tolower(wordToGuess[i]))
        {
            letterColorOnBoard[row][i] = COLOR_CORRECT;
            letterColor[letterToInt(letters[row][i])] = COLOR_CORRECT;
        }
        else if (wordToGuess.find(tolower(letters[row][i])) != std::string::npos)
        {
            letterColorOnBoard[row][i] = COLOR_WRONGSPOT;
            if (letterColor[letterToInt(letters[row][i])] != COLOR_CORRECT)
                letterColor[letterToInt(letters[row][i])] = COLOR_WRONGSPOT;
        }
        else
        {
            letterColorOnBoard[row][i] = COLOR_WRONGLETTER;
            letterColor[letterToInt(letters[row][i])] = COLOR_WRONGLETTER;
        }
    }

    if (isWordCorrect())
        finish();

    completedRows++;
    update();
}

void GameWindow::finish()
{
    if (finished)
        return;
    finished = true;
    lastRow = keyboardWindow->getCurrentRow() - 1;
}

void GameWindow::destroy()
{
    if(hWnd != NULL)
        DestroyWindow(hWnd);
    reset();
    hWnd = NULL;
}

ATOM GameWindow::registerClass(HINSTANCE hInst)
{
    WNDCLASSEXW wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = GameWndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInst;
    wcex.hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_WORDLE));
    wcex.hCursor = nullptr;
    wcex.hbrBackground = CreateSolidBrush(COLOR_BACKGROUND);
    wcex.lpszMenuName = nullptr;
    wcex.lpszClassName = GAME_CLASS;
    wcex.hIconSm = nullptr;
    return RegisterClassExW(&wcex);
}

int GameWindow::getLetterColor(char c)
{
    return letterColor[letterToInt(c)];
}

void GameWindow::drawAnimatedRow(int row)
{
    int animationTime = keyboardWindow->getAnimationTime(row);
    for (int i = 0; i < BOARD_WIDTH; i++)
    {
        int startTime = TILE_ANIMATION_DURATION / 2 * i;
        
        //animation hasn't started
        if (startTime >= animationTime)
            drawTile(row, i, COLOR_DEFAULT);
        //animation has finished
        else if(startTime + TILE_ANIMATION_DURATION <= animationTime)
            drawTile(row, i, letterColorOnBoard[row][i]);
        else
        {
            int timePassed = animationTime - startTime;
            int x = getTileX(i);
            int y = getTileY(row);
            //first phase - rectangle getting smaller
            if (animationTime - startTime < TILE_ANIMATION_DURATION / 2)
            {
                int rectHeight = TILE_SIZE * (TILE_ANIMATION_DURATION / 2 - timePassed) / (TILE_ANIMATION_DURATION / 2);
                drawRect(x, y + (TILE_SIZE - rectHeight) / 2, TILE_SIZE, rectHeight, COLOR_DEFAULT);
                drawLetter(x, y, letters[row][i]);
            }
            else
            {
                int rectHeight = TILE_SIZE * (timePassed - TILE_ANIMATION_DURATION / 2) / (TILE_ANIMATION_DURATION / 2);
                drawRect(x, y + (TILE_SIZE - rectHeight) / 2, TILE_SIZE, rectHeight, letterColorOnBoard[row][i]);
                drawLetter(x, y, letters[row][i]);
            }
        }
    }
}

void GameWindow::drawTile(int row, int column, int color)
{
    int x = getTileX(column);
    int y = getTileY(row);
    drawRect(x, y, TILE_SIZE, TILE_SIZE, color);
    drawLetter(x, y, letters[row][column]);
}

int GameWindow::getTileX(int column)
{
    return MARGIN + column * (MARGIN + TILE_SIZE);
}

int GameWindow::getTileY(int row)
{
    return MARGIN + row * (MARGIN + TILE_SIZE);
}